//
// Created by albert.terlikpayev on 16.10.2026.
//

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "modes/interactive/builtincmd/builtin_cmd.h"
#include "modes/interactive/common/command.h"
#include "modes/interactive/common/error.h"

static const char *builtin_cmd_names[] = {
    [CMD_EXIT] = "exit",
    [CMD_CD] = "cd",
    [CMD_PATH] = "path",
};

static void exec_exit(command_t *cmd);
static void exec_cd(command_t *cmd);
static void exec_path(command_t *cmd);

int find_builtin_cmd(const char *cmd_name) {
    for (int i = 0; i < CMD_COUNT; i++) {
        if (strcmp(cmd_name, builtin_cmd_names[i]) == 0) return i;
    }

    return -1;
}

void exec_builtin_cmd(command_t *cmd) {
    switch (cmd->builtin_cmd_num) {
        case CMD_EXIT:
            exec_exit(cmd);
        case CMD_CD:
            exec_cd(cmd);
        case CMD_PATH:
            exec_path(cmd);
        default: { }
    }
}

static void exec_exit(command_t *cmd) {
    if (cmd->argc > 1) {
        print_error();
        return;
    }
    exit(0);
}

static void exec_cd(command_t *cmd) {
    if (cmd->argc > 2 || cmd->argc < 1) {
        print_error();
        return;
    }

    const char *dir_path = cmd->argv[cmd->argc - 1];
    if (access(dir_path, X_OK) != 0) {
        print_error();
        return;
    }
    if (chdir(dir_path) != 0) print_error();
}

static void exec_path(command_t *cmd) {

}