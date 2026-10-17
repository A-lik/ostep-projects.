//
// Created by albert.terlikpayev on 16.10.2026.
//

#ifndef PROCESSES_SHELL_BUILTIN_CMD_H
#define PROCESSES_SHELL_BUILTIN_CMD_H
#include "modes/interactive/common/command.h"

int find_builtin_cmd(const char *cmd_name);
int exec_builtin_cmd(command_t *cmd);
#endif //PROCESSES_SHELL_BUILTIN_CMD_H
