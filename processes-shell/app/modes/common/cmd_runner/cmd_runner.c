//
// Created by albert.terlikpayev on 17.10.2026.
//

#include "cmd_runner.h"

#include <string.h>

#include "app/modes/common/cmd_executor/builtin/builtin_cmd.h"
#include "app/modes/common/cmd_executor/external/external_cmd.h"

static const char *builtin_cmd_names[] = {
    [CMD_EXIT] = "exit",
    [CMD_CD] = "cd",
    [CMD_PATH] = "path",
};

static int find_builtin_cmd(const char *cmd_name);

int run_cmd(command_t *cmd) {
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