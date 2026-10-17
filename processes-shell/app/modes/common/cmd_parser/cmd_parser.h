//
// Created by albert.terlikpayev on 17.10.2026.
//

#ifndef WISH_CMD_PARSER_H
#define WISH_CMD_PARSER_H
#include "app/modes/common/models/command.h"
#define DELIM " \t\n"


void parse_cmd(command_t *cmd, char **pCmd_line);
#endif //WISH_CMD_PARSER_H
