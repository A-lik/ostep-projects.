//
// Created by albert.terlikpayev on 17.10.2026.
//

#ifndef WISH_CMD_RUNNER_H
#define WISH_CMD_RUNNER_H

#include <sys/_types/_pid_t.h>

#include "app/modes/common/models/command.h"
void run_cmds(commands_t *cmds);
pid_t run_cmd(command_t *cmd);

#endif //WISH_CMD_RUNNER_H
