//
// Created by albert.terlikpayev on 16.10.2026.
//

#include "modes/interactive/builtincmd/builtin_cmd.h"
#include "modes/interactive/command.h"
#include <string.h>
#include <stdlib.h>

static const char *builtin_cmd_names[] = {
    [CMD_EXIT] = "exit",
    [CMD_CD] = "cd",
    [CMD_PATH] = "path",
};

int find_builtin_cmd(const char *cmd_name) {
    for (int i = 0; i < CMD_COUNT; i++) {
        if (strcmp(cmd_name, builtin_cmd_names[i]) == 0) return i;
    }

    return -1;
}

void exec_builtin_cmd(command_t *cmd) {
    switch (cmd->builtin_cmd_num) {
        case CMD_EXIT:
            exit(0);
        case CMD_CD: { }
        case CMD_PATH: { }
        default: { }
    }
}