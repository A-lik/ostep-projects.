//
// Created by albert.terlikpayev on 17.10.2026.
//

#include "cmd_parser.h"

#include <stddef.h>
#include <_string.h>

void parse_cmd(command_t *cmd, char **pCmd_line) {
    char *tok;

    while ((tok = strsep(pCmd_line, DELIM)) != NULL && cmd->argc < 63) {
        if (*tok == '\0') continue;
        cmd->argv[cmd->argc++] = tok;
    }
    cmd->argv[cmd->argc] = NULL;
}
