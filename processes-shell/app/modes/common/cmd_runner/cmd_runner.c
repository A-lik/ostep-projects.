//
// Created by albert.terlikpayev on 17.10.2026.
//

#include "cmd_runner.h"

#include <string.h>
#include <sys/wait.h>
#include <sys/_types/_pid_t.h>

#include "app/modes/common/cmd_executor/builtin/builtin_cmd.h"
#include "app/modes/common/cmd_executor/external/external_cmd.h"
#include "app/modes/common/error/error.h"

static const char *builtin_cmd_names[] = {
    [CMD_EXIT] = "exit",
    [CMD_CD] = "cd",
    [CMD_PATH] = "path",
};

static int find_builtin_cmd(const char *cmd_name);

void run_cmds(commands_t *cmds) {
    pid_t pids[64];
    int pid_count = 0;
    for (int i = 0; i < cmds->cmd_num; i++) {
        pid_t pid = run_cmd(&cmds->parallel_cmds[i]);
        if (pid < 0) print_error();
        if (pid > 0) pids[pid_count++] = pid;
    }

    for (int i = 0; i < pid_count; i++) {
        waitpid(pids[i], NULL, 0);
    }
}


pid_t run_cmd(command_t *cmd) {
    const int builtin = find_builtin_cmd(cmd->argv[0]);
    if (builtin < 0) return exec_external_cmd(cmd);

    cmd->builtin_cmd_num = builtin;
    return exec_builtin_cmd(cmd);
}

static int find_builtin_cmd(const char *cmd_name) {
    for (int i = 0; i < CMD_COUNT; i++) {
        if (strcmp(cmd_name, builtin_cmd_names[i]) == 0) return i;
    }

    return -1;
}