/*
 * procinfo.c - dump everything a Linux process can learn about itself.
 *
 * Build:  gcc -Wall -Wextra -O2 -o procinfo procinfo.c
 * Run:    ./procinfo [args...]        (pass args to see them echoed back)
 *
 * Every section is independent; delete what you don't need.
 */

#define _GNU_SOURCE

#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <limits.h>
#include <malloc.h>
#include <pwd.h>
#include <sched.h>
#include <signal.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/auxv.h>
#include <sys/personality.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <time.h>
#include <unistd.h>

extern char **environ;

/* ------------------------------------------------------------------ */
/* small helpers                                                       */
/* ------------------------------------------------------------------ */

static void section(const char *name)
{
    printf("\n\033[1m=== %s ", name);
    for (size_t i = strlen(name); i < 60; i++)
        putchar('=');
    printf("\033[0m\n");
}

static void kv(const char *k, const char *fmt, ...)
{
    va_list ap;
    printf("  %-28s ", k);
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    putchar('\n');
}

/* read a whole small file into a caller-supplied buffer, NUL-terminate */
static ssize_t slurp(const char *path, char *buf, size_t cap)
{
    int fd = open(path, O_RDONLY);
    if (fd < 0)
        return -1;
    ssize_t n = read(fd, buf, cap - 1);
    close(fd);
    if (n < 0)
        return -1;
    buf[n] = '\0';
    return n;
}

/* print a whole /proc file verbatim, indented */
static void dump_file(const char *path, int max_lines)
{
    FILE *f = fopen(path, "r");
    if (!f) {
        printf("  (cannot open %s: %s)\n", path, strerror(errno));
        return;
    }
    char line[4096];
    int n = 0;
    while (fgets(line, sizeof line, f)) {
        if (max_lines > 0 && n >= max_lines) {
            printf("  ... (truncated)\n");
            break;
        }
        printf("  %s", line);
        n++;
    }
    fclose(f);
}

static void print_symlink(const char *label, const char *path)
{
    char buf[PATH_MAX];
    ssize_t n = readlink(path, buf, sizeof buf - 1);
    if (n < 0) {
        kv(label, "(unavailable: %s)", strerror(errno));
        return;
    }
    buf[n] = '\0';
    kv(label, "%s", buf);
}

static const char *timespec_str(const struct timespec *ts, char *out, size_t cap)
{
    snprintf(out, cap, "%lld.%09ld", (long long)ts->tv_sec, ts->tv_nsec);
    return out;
}

/* ------------------------------------------------------------------ */
/* 1. identity and credentials                                         */
/* ------------------------------------------------------------------ */

static void show_identity(void)
{
    section("IDENTITY");

    kv("pid",  "%d", getpid());
    kv("ppid", "%d", getppid());
    kv("tid (gettid)", "%ld", (long)syscall(SYS_gettid));
    kv("pgid", "%d", getpgrp());
    kv("sid",  "%d", getsid(0));

    char comm[256];
    if (slurp("/proc/self/comm", comm, sizeof comm) > 0) {
        comm[strcspn(comm, "\n")] = '\0';
        kv("comm (thread name)", "%s", comm);
    }

    /* foreground process group of the controlling terminal */
    pid_t fg = tcgetpgrp(STDIN_FILENO);
    if (fg >= 0)
        kv("tty foreground pgid", "%d%s", fg,
           fg == getpgrp() ? "  (that's us)" : "  (we're background)");
    else
        kv("tty foreground pgid", "n/a (%s)", strerror(errno));

    const char *tty = ttyname(STDIN_FILENO);
    kv("controlling tty", "%s", tty ? tty : "none");

    section("CREDENTIALS");

    uid_t ruid, euid, suid;
    gid_t rgid, egid, sgid;
    getresuid(&ruid, &euid, &suid);
    getresgid(&rgid, &egid, &sgid);

    struct passwd *pw = getpwuid(ruid);
    kv("uid  real/eff/saved", "%u / %u / %u   (%s)", ruid, euid, suid,
       pw ? pw->pw_name : "?");

    struct group *gr = getgrgid(rgid);
    kv("gid  real/eff/saved", "%u / %u / %u   (%s)", rgid, egid, sgid,
       gr ? gr->gr_name : "?");

    int ngroups = getgroups(0, NULL);
    if (ngroups > 0) {
        gid_t *groups = calloc(ngroups, sizeof *groups);
        if (getgroups(ngroups, groups) < 0)
            perror("getgroups");
        printf("  %-28s ", "supplementary groups");
        for (int i = 0; i < ngroups; i++) {
            struct group *g = getgrgid(groups[i]);
            printf("%u(%s)%s", groups[i], g ? g->gr_name : "?",
                   i + 1 < ngroups ? " " : "");
        }
        putchar('\n');
        free(groups);
    }

    mode_t old = umask(0);
    umask(old);
    kv("umask", "0%03o", old);

    char loginuid[64];
    if (slurp("/proc/self/loginuid", loginuid, sizeof loginuid) > 0)
        kv("loginuid (audit)", "%s", loginuid);
}

/* ------------------------------------------------------------------ */
/* 2. paths, arguments, environment                                    */
/* ------------------------------------------------------------------ */

static void show_paths(int argc, char **argv)
{
    section("PATHS");

    print_symlink("executable  (/proc/self/exe)",  "/proc/self/exe");
    print_symlink("cwd         (/proc/self/cwd)",  "/proc/self/cwd");
    print_symlink("root        (/proc/self/root)", "/proc/self/root");

    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof cwd))
        kv("getcwd()", "%s", cwd);

    section("ARGUMENTS");
    kv("argc", "%d", argc);
    for (int i = 0; i < argc; i++)
        printf("  argv[%d] = \"%s\"\n", i, argv[i]);
    printf("  argv[%d] = NULL\n", argc);

    section("ENVIRONMENT");
    int n = 0;
    for (char **e = environ; *e; e++)
        n++;
    kv("variable count", "%d", n);
    for (char **e = environ; *e; e++)
        printf("    %s\n", *e);
}

/* ------------------------------------------------------------------ */
/* 3. auxiliary vector                                                 */
/* ------------------------------------------------------------------ */

static void show_auxv(void)
{
    section("AUXILIARY VECTOR (kernel -> loader handoff)");

    kv("AT_PAGESZ  page size",     "%lu bytes", getauxval(AT_PAGESZ));
    kv("AT_CLKTCK  clock ticks",   "%lu Hz",    getauxval(AT_CLKTCK));
    kv("AT_PHDR    prog headers",  "0x%lx",     getauxval(AT_PHDR));
    kv("AT_PHNUM   phdr count",    "%lu",       getauxval(AT_PHNUM));
    kv("AT_ENTRY   entry point",   "0x%lx",     getauxval(AT_ENTRY));
    kv("AT_BASE    interp base",   "0x%lx",     getauxval(AT_BASE));
    kv("AT_UID/EUID",              "%lu / %lu", getauxval(AT_UID),
                                                getauxval(AT_EUID));
    kv("AT_SECURE  setuid-ish?",   "%lu",       getauxval(AT_SECURE));
    kv("AT_HWCAP   cpu features",  "0x%lx",     getauxval(AT_HWCAP));
    kv("AT_HWCAP2",                "0x%lx",     getauxval(AT_HWCAP2));

    const char *plat = (const char *)getauxval(AT_PLATFORM);
    kv("AT_PLATFORM", "%s", plat ? plat : "?");

    unsigned long rnd = getauxval(AT_RANDOM);
    if (rnd) {
        unsigned char *b = (unsigned char *)rnd;
        printf("  %-28s ", "AT_RANDOM 16 seed bytes");
        for (int i = 0; i < 16; i++)
            printf("%02x", b[i]);
        printf("  (stack canary source)\n");
    }
}

/* ------------------------------------------------------------------ */
/* 4. host and kernel                                                  */
/* ------------------------------------------------------------------ */

static void show_system(void)
{
    section("HOST / KERNEL");

    struct utsname u;
    if (uname(&u) == 0) {
        kv("sysname",  "%s", u.sysname);
        kv("nodename", "%s", u.nodename);
        kv("release",  "%s", u.release);
        kv("version",  "%s", u.version);
        kv("machine",  "%s", u.machine);
        kv("domainname", "%s", u.domainname);
    }

    kv("_SC_NPROCESSORS_CONF",  "%ld", sysconf(_SC_NPROCESSORS_CONF));
    kv("_SC_NPROCESSORS_ONLN",  "%ld", sysconf(_SC_NPROCESSORS_ONLN));
    kv("_SC_PAGESIZE",          "%ld", sysconf(_SC_PAGESIZE));
    kv("_SC_CLK_TCK",           "%ld", sysconf(_SC_CLK_TCK));
    kv("_SC_PHYS_PAGES",        "%ld", sysconf(_SC_PHYS_PAGES));
    kv("_SC_AVPHYS_PAGES",      "%ld", sysconf(_SC_AVPHYS_PAGES));
    kv("_SC_OPEN_MAX",          "%ld", sysconf(_SC_OPEN_MAX));
    kv("_SC_ARG_MAX",           "%ld", sysconf(_SC_ARG_MAX));
    kv("_SC_CHILD_MAX",         "%ld", sysconf(_SC_CHILD_MAX));
    kv("_SC_LEVEL1_DCACHE_SIZE","%ld", sysconf(_SC_LEVEL1_DCACHE_SIZE));
    kv("_SC_LEVEL1_DCACHE_LINESIZE", "%ld",
       sysconf(_SC_LEVEL1_DCACHE_LINESIZE));
    kv("_SC_LEVEL2_CACHE_SIZE", "%ld", sysconf(_SC_LEVEL2_CACHE_SIZE));
    kv("_SC_LEVEL3_CACHE_SIZE", "%ld", sysconf(_SC_LEVEL3_CACHE_SIZE));

    int pers = personality(0xffffffff);
    if (pers != -1)
        kv("personality", "0x%08x%s", pers,
           (pers & ADDR_NO_RANDOMIZE) ? "  (ASLR disabled!)" : "");
}

/* ------------------------------------------------------------------ */
/* 5. scheduling                                                       */
/* ------------------------------------------------------------------ */

static const char *sched_policy_name(int p)
{
    switch (p & ~SCHED_RESET_ON_FORK) {
    case SCHED_OTHER:  return "SCHED_OTHER (CFS)";
    case SCHED_FIFO:   return "SCHED_FIFO (realtime)";
    case SCHED_RR:     return "SCHED_RR (realtime)";
    case SCHED_BATCH:  return "SCHED_BATCH";
    case SCHED_IDLE:   return "SCHED_IDLE";
    case SCHED_DEADLINE: return "SCHED_DEADLINE";
    default:           return "unknown";
    }
}

static void show_scheduling(void)
{
    section("SCHEDULING");

    int pol = sched_getscheduler(0);
    kv("policy", "%s", sched_policy_name(pol));

    struct sched_param sp;
    if (sched_getparam(0, &sp) == 0)
        kv("rt priority", "%d  (range %d..%d for this policy)",
           sp.sched_priority,
           sched_get_priority_min(pol), sched_get_priority_max(pol));

    errno = 0;
    int nice_val = getpriority(PRIO_PROCESS, 0);
    if (errno == 0)
        kv("nice", "%d  (lower = more CPU)", nice_val);

    struct timespec q;
    if (sched_rr_get_interval(0, &q) == 0) {
        char b[64];
        kv("RR timeslice", "%s s", timespec_str(&q, b, sizeof b));
    }

    cpu_set_t set;
    CPU_ZERO(&set);
    if (sched_getaffinity(0, sizeof set, &set) == 0) {
        printf("  %-28s ", "cpu affinity mask");
        int count = 0;
        for (int i = 0; i < CPU_SETSIZE; i++)
            if (CPU_ISSET(i, &set)) {
                printf("%d ", i);
                count++;
            }
        printf(" (%d cpus allowed)\n", count);
    }

    kv("currently running on cpu", "%d", sched_getcpu());
}

/* ------------------------------------------------------------------ */
/* 6. time                                                             */
/* ------------------------------------------------------------------ */

static void show_time(void)
{
    section("TIME AND CLOCKS");

    struct timespec ts;
    char b[64];

    static const struct { clockid_t id; const char *name; } clocks[] = {
        { CLOCK_REALTIME,           "CLOCK_REALTIME" },
        { CLOCK_MONOTONIC,          "CLOCK_MONOTONIC" },
        { CLOCK_MONOTONIC_RAW,      "CLOCK_MONOTONIC_RAW" },
        { CLOCK_BOOTTIME,           "CLOCK_BOOTTIME (incl. suspend)" },
        { CLOCK_PROCESS_CPUTIME_ID, "CLOCK_PROCESS_CPUTIME_ID" },
        { CLOCK_THREAD_CPUTIME_ID,  "CLOCK_THREAD_CPUTIME_ID" },
    };

    for (size_t i = 0; i < sizeof clocks / sizeof *clocks; i++) {
        if (clock_gettime(clocks[i].id, &ts) == 0)
            kv(clocks[i].name, "%s s", timespec_str(&ts, b, sizeof b));
        if (clock_getres(clocks[i].id, &ts) == 0)
            kv("  ^ resolution", "%s s", timespec_str(&ts, b, sizeof b));
    }

    struct tms t;
    clock_t real = times(&t);
    long hz = sysconf(_SC_CLK_TCK);
    kv("times() elapsed", "%.3f s", (double)real / hz);
    kv("times() self  user/sys", "%.3f / %.3f s",
       (double)t.tms_utime / hz, (double)t.tms_stime / hz);
    kv("times() child user/sys", "%.3f / %.3f s",
       (double)t.tms_cutime / hz, (double)t.tms_cstime / hz);

    /* field 22 of /proc/self/stat is starttime, in clock ticks since boot */
    char stat[4096];
    if (slurp("/proc/self/stat", stat, sizeof stat) > 0) {
        char *p = strrchr(stat, ')');   /* comm may contain spaces+parens */
        if (p) {
            p += 2;
            unsigned long long starttime = 0;
            int field = 3;
            char *tok = strtok(p, " ");
            while (tok) {
                if (field == 22) {
                    starttime = strtoull(tok, NULL, 10);
                    break;
                }
                field++;
                tok = strtok(NULL, " ");
            }
            kv("start time since boot", "%.2f s", (double)starttime / hz);
        }
    }

    char up[128];
    if (slurp("/proc/uptime", up, sizeof up) > 0) {
        up[strcspn(up, "\n")] = '\0';
        kv("system uptime / idle", "%s s", up);
    }
}

/* ------------------------------------------------------------------ */
/* 7. resource limits                                                  */
/* ------------------------------------------------------------------ */

static void print_limit_value(rlim_t v)
{
    if (v == RLIM_INFINITY)
        printf("%-20s", "unlimited");
    else
        printf("%-20llu", (unsigned long long)v);
}

static void show_limits(void)
{
    section("RESOURCE LIMITS (getrlimit)");

    static const struct { int id; const char *name; const char *unit; } lims[] = {
        { RLIMIT_AS,         "RLIMIT_AS",         "bytes of address space" },
        { RLIMIT_CORE,       "RLIMIT_CORE",       "bytes of core dump" },
        { RLIMIT_CPU,        "RLIMIT_CPU",        "seconds of CPU" },
        { RLIMIT_DATA,       "RLIMIT_DATA",       "bytes of data segment" },
        { RLIMIT_FSIZE,      "RLIMIT_FSIZE",      "bytes per file" },
        { RLIMIT_LOCKS,      "RLIMIT_LOCKS",      "file locks" },
        { RLIMIT_MEMLOCK,    "RLIMIT_MEMLOCK",    "bytes lockable in RAM" },
        { RLIMIT_MSGQUEUE,   "RLIMIT_MSGQUEUE",   "bytes in POSIX mqueues" },
        { RLIMIT_NICE,       "RLIMIT_NICE",       "nice ceiling (20-n)" },
        { RLIMIT_NOFILE,     "RLIMIT_NOFILE",     "open file descriptors" },
        { RLIMIT_NPROC,      "RLIMIT_NPROC",      "processes per uid" },
        { RLIMIT_RSS,        "RLIMIT_RSS",        "resident pages (ignored)" },
        { RLIMIT_RTPRIO,     "RLIMIT_RTPRIO",     "realtime priority" },
        { RLIMIT_RTTIME,     "RLIMIT_RTTIME",     "us of RT CPU w/o block" },
        { RLIMIT_SIGPENDING, "RLIMIT_SIGPENDING", "queued signals" },
        { RLIMIT_STACK,      "RLIMIT_STACK",      "bytes of stack" },
    };

    printf("  %-20s %-20s %-20s %s\n", "LIMIT", "SOFT", "HARD", "UNIT");
    for (size_t i = 0; i < sizeof lims / sizeof *lims; i++) {
        struct rlimit rl;
        if (getrlimit(lims[i].id, &rl) != 0)
            continue;
        printf("  %-20s ", lims[i].name);
        print_limit_value(rl.rlim_cur);
        printf(" ");
        print_limit_value(rl.rlim_max);
        printf(" %s\n", lims[i].unit);
    }
}

/* ------------------------------------------------------------------ */
/* 8. resource usage                                                   */
/* ------------------------------------------------------------------ */

static void print_rusage(const char *label, int who)
{
    struct rusage ru;
    if (getrusage(who, &ru) != 0) {
        printf("  %s: unavailable (%s)\n", label, strerror(errno));
        return;
    }
    printf("\n  \033[4m%s\033[0m\n", label);
    kv("user CPU time",       "%ld.%06ld s", (long)ru.ru_utime.tv_sec,
                                             (long)ru.ru_utime.tv_usec);
    kv("system CPU time",     "%ld.%06ld s", (long)ru.ru_stime.tv_sec,
                                             (long)ru.ru_stime.tv_usec);
    kv("max RSS (peak)",      "%ld KB", ru.ru_maxrss);
    kv("minor page faults",   "%ld  (served from memory)", ru.ru_minflt);
    kv("major page faults",   "%ld  (needed disk I/O)",    ru.ru_majflt);
    kv("block input ops",     "%ld", ru.ru_inblock);
    kv("block output ops",    "%ld", ru.ru_oublock);
    kv("voluntary ctx switch","%ld  (blocked on something)", ru.ru_nvcsw);
    kv("involuntary ctx sw",  "%ld  (preempted)",            ru.ru_nivcsw);
    kv("signals received",    "%ld", ru.ru_nsignals);
    kv("IPC messages sent/rcv","%ld / %ld", ru.ru_msgsnd, ru.ru_msgrcv);
}

static void show_usage(void)
{
    section("RESOURCE USAGE (getrusage)");
    print_rusage("RUSAGE_SELF (all threads)", RUSAGE_SELF);
    print_rusage("RUSAGE_THREAD (this thread only)", RUSAGE_THREAD);
    print_rusage("RUSAGE_CHILDREN (reaped children)", RUSAGE_CHILDREN);
}

/* ------------------------------------------------------------------ */
/* 9. memory                                                           */
/* ------------------------------------------------------------------ */

static void show_memory(void)
{
    section("MEMORY");

    long page = sysconf(_SC_PAGESIZE);
    char buf[512];

    if (slurp("/proc/self/statm", buf, sizeof buf) > 0) {
        unsigned long size, res, shared, text, lib, data, dt;
        if (sscanf(buf, "%lu %lu %lu %lu %lu %lu %lu",
                   &size, &res, &shared, &text, &lib, &data, &dt) >= 6) {
            printf("  \033[4m/proc/self/statm (current, cheap to poll)\033[0m\n");
            kv("virtual size",    "%lu KB", size   * page / 1024);
            kv("resident (RSS)",  "%lu KB", res    * page / 1024);
            kv("shared pages",    "%lu KB", shared * page / 1024);
            kv("text (code)",     "%lu KB", text   * page / 1024);
            kv("data + stack",    "%lu KB", data   * page / 1024);
        }
    }

#if defined(__GLIBC__) && defined(__GLIBC_PREREQ)
#if __GLIBC_PREREQ(2, 33)
    printf("\n  \033[4mglibc allocator (mallinfo2)\033[0m\n");
    struct mallinfo2 mi = mallinfo2();
    kv("arena (sbrk heap)",   "%zu bytes", mi.arena);
    kv("hblkhd (mmap'd)",     "%zu bytes in %zu blocks", mi.hblkhd, mi.hblks);
    kv("uordblks (in use)",   "%zu bytes", mi.uordblks);
    kv("fordblks (free)",     "%zu bytes", mi.fordblks);
    kv("keepcost (trimmable)","%zu bytes", mi.keepcost);
#endif
#endif

    printf("\n  \033[4mstack and break pointers\033[0m\n");
    int local;
    kv("address of a local var", "%p  (stack)", (void *)&local);
    kv("current program break",  "%p  (heap top)", sbrk(0));
    void *heap = malloc(1);
    kv("address from malloc(1)", "%p", heap);
    free(heap);
    kv("address of show_memory", "%p  (text)", (void *)(uintptr_t)show_memory);
    kv("address of environ",     "%p  (data)", (void *)&environ);

    printf("\n  \033[4m/proc/self/maps (virtual address space)\033[0m\n");
    dump_file("/proc/self/maps", 40);

    printf("\n  \033[4m/proc/self/smaps_rollup (aggregated PSS etc.)\033[0m\n");
    dump_file("/proc/self/smaps_rollup", 0);
}

/* ------------------------------------------------------------------ */
/* 10. signals                                                         */
/* ------------------------------------------------------------------ */

static void show_signals(void)
{
    section("SIGNALS");

    sigset_t blocked, pending;
    sigemptyset(&blocked);
    sigprocmask(SIG_SETMASK, NULL, &blocked);
    sigpending(&pending);

    printf("  %-4s %-12s %-10s %-9s %s\n",
           "NUM", "NAME", "DISPOSITION", "BLOCKED", "PENDING");

    for (int s = 1; s < NSIG; s++) {
        const char *name = sigabbrev_np(s);
        if (!name)
            continue;

        struct sigaction sa;
        const char *disp = "?";
        if (sigaction(s, NULL, &sa) == 0) {
            if (sa.sa_handler == SIG_DFL)      disp = "default";
            else if (sa.sa_handler == SIG_IGN) disp = "ignored";
            else                                disp = "handler";
        } else {
            disp = "uncatchable";
        }

        printf("  %-4d SIG%-9s %-10s %-9s %s\n", s, name, disp,
               sigismember(&blocked, s) ? "yes" : "-",
               sigismember(&pending, s) ? "yes" : "-");
    }

    printf("\n  Note: SIGKILL(9) and SIGSTOP(19) report \"default\" because\n"
           "  querying them is allowed - it is *installing* a handler that\n"
           "  fails with EINVAL. They can never be caught, blocked or ignored.\n"
           "  Signals 32 and 33 are reserved by NPTL and have no name.\n");
}

/* ------------------------------------------------------------------ */
/* 11. open file descriptors                                           */
/* ------------------------------------------------------------------ */

static const char *file_type_str(mode_t m)
{
    if (S_ISREG(m))  return "file";
    if (S_ISDIR(m))  return "dir";
    if (S_ISCHR(m))  return "chardev";
    if (S_ISBLK(m))  return "blockdev";
    if (S_ISFIFO(m)) return "fifo";
    if (S_ISLNK(m))  return "symlink";
    if (S_ISSOCK(m)) return "socket";
    return "?";
}

static void show_fds(void)
{
    section("OPEN FILE DESCRIPTORS");

    DIR *d = opendir("/proc/self/fd");
    if (!d) {
        printf("  cannot open /proc/self/fd: %s\n", strerror(errno));
        return;
    }

    printf("  %-4s %-9s %-8s %-10s %s\n",
           "FD", "TYPE", "FLAGS", "CLOEXEC", "TARGET");

    struct dirent *e;
    int count = 0;
    while ((e = readdir(d))) {
        if (e->d_name[0] == '.')
            continue;
        int fd = atoi(e->d_name);

        char link[PATH_MAX], path[PATH_MAX];
        snprintf(path, sizeof path, "/proc/self/fd/%s", e->d_name);
        ssize_t n = readlink(path, link, sizeof link - 1);
        if (n < 0)
            continue;
        link[n] = '\0';

        struct stat st;
        const char *type = "?";
        if (fstat(fd, &st) == 0)
            type = file_type_str(st.st_mode);

        int fl = fcntl(fd, F_GETFL);
        char flags[32] = "?";
        if (fl >= 0) {
            const char *acc = (fl & O_ACCMODE) == O_RDONLY ? "r" :
                              (fl & O_ACCMODE) == O_WRONLY ? "w" : "rw";
            snprintf(flags, sizeof flags, "%s%s%s", acc,
                     (fl & O_APPEND)   ? ",app"  : "",
                     (fl & O_NONBLOCK) ? ",nonb" : "");
        }

        int fdf = fcntl(fd, F_GETFD);
        printf("  %-4d %-9s %-8s %-10s %s\n", fd, type, flags,
               fdf >= 0 ? ((fdf & FD_CLOEXEC) ? "yes" : "no") : "?", link);
        count++;
    }
    closedir(d);

    /* the readdir fd itself is now closed, so subtract it */
    kv("total open (approx)", "%d", count - 1);
}

/* ------------------------------------------------------------------ */
/* 12. threads                                                         */
/* ------------------------------------------------------------------ */

static void show_threads(void)
{
    section("THREADS");

    DIR *d = opendir("/proc/self/task");
    if (!d) {
        printf("  unavailable\n");
        return;
    }
    struct dirent *e;
    while ((e = readdir(d))) {
        if (e->d_name[0] == '.')
            continue;
        char path[PATH_MAX], name[256];
        snprintf(path, sizeof path, "/proc/self/task/%s/comm", e->d_name);
        if (slurp(path, name, sizeof name) > 0) {
            name[strcspn(name, "\n")] = '\0';
            printf("  tid %-8s comm=%s\n", e->d_name, name);
        }
    }
    closedir(d);

    pthread_attr_t attr;
    if (pthread_getattr_np(pthread_self(), &attr) == 0) {
        void *stackaddr;
        size_t stacksize, guardsize;
        pthread_attr_getstack(&attr, &stackaddr, &stacksize);
        pthread_attr_getguardsize(&attr, &guardsize);
        kv("stack base", "%p", stackaddr);
        kv("stack size", "%zu bytes (%zu KB)", stacksize, stacksize / 1024);
        kv("guard size", "%zu bytes", guardsize);
        pthread_attr_destroy(&attr);
    }
}

/* ------------------------------------------------------------------ */
/* 13. security context                                                */
/* ------------------------------------------------------------------ */

static const char *cap_names[] = {
    "CHOWN", "DAC_OVERRIDE", "DAC_READ_SEARCH", "FOWNER", "FSETID",
    "KILL", "SETGID", "SETUID", "SETPCAP", "LINUX_IMMUTABLE",
    "NET_BIND_SERVICE", "NET_BROADCAST", "NET_ADMIN", "NET_RAW",
    "IPC_LOCK", "IPC_OWNER", "SYS_MODULE", "SYS_RAWIO", "SYS_CHROOT",
    "SYS_PTRACE", "SYS_PACCT", "SYS_ADMIN", "SYS_BOOT", "SYS_NICE",
    "SYS_RESOURCE", "SYS_TIME", "SYS_TTY_CONFIG", "MKNOD", "LEASE",
    "AUDIT_WRITE", "AUDIT_CONTROL", "SETFCAP", "MAC_OVERRIDE",
    "MAC_ADMIN", "SYSLOG", "WAKE_ALARM", "BLOCK_SUSPEND", "AUDIT_READ",
    "PERFMON", "BPF", "CHECKPOINT_RESTORE",
};

static void decode_caps(const char *label, unsigned long long mask)
{
    printf("  %-28s 0x%016llx\n", label, mask);
    if (mask == 0) {
        printf("      (none)\n");
        return;
    }
    printf("      ");
    int col = 0;
    for (size_t i = 0; i < sizeof cap_names / sizeof *cap_names; i++) {
        if (mask & (1ULL << i)) {
            printf("%s ", cap_names[i]);
            if (++col % 5 == 0)
                printf("\n      ");
        }
    }
    printf("\n");
}

static void show_security(void)
{
    section("SECURITY CONTEXT");

    FILE *f = fopen("/proc/self/status", "r");
    if (f) {
        char line[512];
        while (fgets(line, sizeof line, f)) {
            unsigned long long v;
            if (sscanf(line, "CapEff: %llx", &v) == 1)
                decode_caps("effective capabilities", v);
            else if (sscanf(line, "CapPrm: %llx", &v) == 1)
                decode_caps("permitted capabilities", v);
            else if (sscanf(line, "CapBnd: %llx", &v) == 1)
                printf("  %-28s 0x%016llx\n", "bounding set", v);
            else if (strncmp(line, "NoNewPrivs:", 11) == 0 ||
                     strncmp(line, "Seccomp:", 8) == 0 ||
                     strncmp(line, "Speculation_Store_Bypass:", 25) == 0)
                printf("  %s", line);
        }
        fclose(f);
    }

    char buf[4096];
    if (slurp("/proc/self/attr/current", buf, sizeof buf) > 0)
        kv("LSM label", "%s", buf);

    printf("\n  \033[4mnamespaces (/proc/self/ns)\033[0m\n");
    static const char *ns[] = { "cgroup", "ipc", "mnt", "net", "pid",
                                "time", "user", "uts" };
    for (size_t i = 0; i < sizeof ns / sizeof *ns; i++) {
        char path[PATH_MAX], target[PATH_MAX];
        snprintf(path, sizeof path, "/proc/self/ns/%s", ns[i]);
        ssize_t n = readlink(path, target, sizeof target - 1);
        if (n > 0) {
            target[n] = '\0';
            printf("  %-28s %s\n", ns[i], target);
        }
    }

    printf("\n  \033[4mcgroup membership\033[0m\n");
    dump_file("/proc/self/cgroup", 0);
}

/* ------------------------------------------------------------------ */
/* 14. raw /proc dumps                                                 */
/* ------------------------------------------------------------------ */

static void show_proc_files(void)
{
    section("RAW /proc/self/status");
    dump_file("/proc/self/status", 0);

    section("RAW /proc/self/io  (bytes through read/write syscalls)");
    dump_file("/proc/self/io", 0);

    section("RAW /proc/self/limits");
    dump_file("/proc/self/limits", 0);

    section("RAW /proc/self/sched");
    dump_file("/proc/self/sched", 20);

    section("RAW /proc/self/stat  (the line ps parses)");
    dump_file("/proc/self/stat", 0);
    printf("\n  Field order: pid comm state ppid pgrp session tty_nr tpgid\n"
           "  flags minflt cminflt majflt cmajflt utime stime cutime\n"
           "  cstime priority nice num_threads itrealvalue starttime\n"
           "  vsize rss ... (see man 5 proc)\n");

    section("RAW /proc/self/wchan  (kernel function we're sleeping in)");
    dump_file("/proc/self/wchan", 0);
    printf("\n");
}

/* ------------------------------------------------------------------ */

int main(int argc, char **argv)
{
    printf("\033[1mProcess self-inspection report\033[0m\n");

    show_identity();
    show_paths(argc, argv);
    show_auxv();
    show_system();
    show_scheduling();
    show_time();
    show_limits();
    show_usage();
    show_memory();
    show_signals();
    show_fds();
    show_threads();
    show_security();
    show_proc_files();

    section("DONE");
    printf("  Re-run under different conditions to see values change:\n"
           "    nice -n 10 ./procinfo          # nice value\n"
           "    taskset -c 0,1 ./procinfo      # affinity mask\n"
           "    ulimit -n 128 && ./procinfo    # RLIMIT_NOFILE\n"
           "    ./procinfo < /dev/null > out   # fd targets\n"
           "    setarch -R ./procinfo          # ASLR off, stable maps\n"
           "    unshare -Ur ./procinfo         # new namespaces\n\n");

    return 0;
}