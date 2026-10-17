#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "app/modes/interactive/interactive.h"
#include "common/error.h"
#include "app/modes/interactive/builtincmd/builtin_cmd.h"
#include "app/modes/interactive/externalcmd/external_cmd.h"

#define DELIM " \t\n"

static void print_sh_name(char *prog_name);
static int run_cmd(command_t *cmd);

void interactive(char *prog_name) {
    char *pLine = NULL;
    size_t linecap = 0;
    ssize_t linelen = 0;

    while (1) {
        print_sh_name(prog_name);
        command_t cmd = {0};

        if (getline(&pLine, &linecap, stdin) == -1 ) break;

        char *tok;
        char *ppLine = pLine;

        while ((tok = strsep(&ppLine, DELIM)) != NULL && cmd.argc < 63) {
            if (*tok == '\0') continue;
            cmd.argv[cmd.argc++] = tok;
        }
        cmd.argv[cmd.argc] = NULL;

        if (run_cmd(&cmd) < 0) print_error();
    }

    free(pLine);
}

static void print_sh_name(char *prog_name) {
    printf("%s> ", prog_name);
    fflush(stdout);
}

static int run_cmd(command_t *cmd) {
    const int builtin = find_builtin_cmd(cmd->argv[0]);
    if (builtin < 0) return exec_external_cmd(cmd);

    cmd->builtin_cmd_num = builtin;
    return exec_builtin_cmd(cmd);
}