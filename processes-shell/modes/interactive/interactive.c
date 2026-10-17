#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include "modes/interactive/interactive.h"
#include "modes/interactive/builtincmd/builtin_cmd.h"
#include "modes/interactive/externalcmd/external_cmd.h"

#define DELIM " \t\n"

static void print_sh_name(char *prog_name) {
    printf("%s> ", prog_name);
    fflush(stdout);
}

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

        int cmd_res;
        if ((cmd_res = find_builtin_cmd(cmd.argv[0])) >= 0) {
            cmd.builtin_cmd_num = cmd_res;
            exec_builtin_cmd(&cmd);
        }
        else exec_external_cmd(&cmd);
    }

    free(pLine);
}