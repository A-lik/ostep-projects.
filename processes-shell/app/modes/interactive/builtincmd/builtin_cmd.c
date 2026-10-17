//
// Created by albert.terlikpayev on 16.10.2026.
//

#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "app/modes/interactive/builtincmd/builtin_cmd.h"
#include "app/modes/interactive/common/command.h"
#include "app/path.h"

static const char *builtin_cmd_names[] = {
    [CMD_EXIT] = "exit",
    [CMD_CD] = "cd",
    [CMD_PATH] = "path",
};

static int exec_exit(command_t *cmd);
static int exec_cd(command_t *cmd);
static int exec_path(command_t *cmd);

int find_builtin_cmd(const char *cmd_name) {
    for (int i = 0; i < CMD_COUNT; i++) {
        if (strcmp(cmd_name, builtin_cmd_names[i]) == 0) return i;
    }

    return -1;
}

int exec_builtin_cmd(command_t *cmd) {
    switch (cmd->builtin_cmd_num) {
        case CMD_EXIT:
            return exec_exit(cmd);
        case CMD_CD:
            return exec_cd(cmd);
        case CMD_PATH:
            return exec_path(cmd);
        default:
            return -1;
    }
}

static int exec_exit(command_t *cmd) {
    if (cmd->argc > 1) return -1;
    exit(0);
}

static int exec_cd(command_t *cmd) {
    if (cmd->argc > 2 || cmd->argc < 1) return -1;

    const char *dir_path = cmd->argv[cmd->argc - 1];
    if (access(dir_path, X_OK) != 0) return -1;
    if (chdir(dir_path) != 0) return -1;

    return 0;
}

static int exec_path(command_t *cmd) {
    if (cmd->argc == 1) {
        for (int i = 0; i < search_path_len; i++) {
            free(search_path[i]);
            search_path[i] = NULL;
        }
        search_path_len = 0;
    }

    int new_path_len = 0;
    for (int i = 1; i < cmd->argc; i++) {
        char *pArg = strdup(cmd->argv[i]);
        if (!pArg) return -1;
        search_path[new_path_len++] = pArg; // first arg is prog name e.g. ls, cd...
    }
    search_path_len = new_path_len;

    return 0;
}
