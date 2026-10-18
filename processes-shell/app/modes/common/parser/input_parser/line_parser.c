//
// Created by albert.terlikpayev on 18.10.2026.
//

#include "line_parser.h"

#include <stdio.h>
#include <_string.h>

#include "app/modes/common/tokenizer/tokenizer.h"
#include "app/modes/common/models/command.h"

#define REDIRECT_DELIMITER ">"

parse_status_t parse_line(char *line, command_t *cmd) {
    char *redir = strchr(line, '>');

    if (redir) {
        if (strchr(redir + 1, '>')) return PARSE_ERR;

        *redir = '\0';
        char *files[2];
        const int file_num = tokenize(redir + 1, files, 2);
        if (file_num != 1) return PARSE_ERR;

        cmd->is_redirect = 1;
        cmd->redirect_fName = files[0];
    }

    int max = (int)(sizeof cmd->argv / sizeof cmd->argv[0]) - 1;
    cmd->argc = tokenize(line, cmd->argv, max);
    if (cmd->argc > max) return PARSE_ERR;
    if (cmd->argc == 0) return redir ? PARSE_ERR : PARSE_EMPTY;
    cmd->argv[cmd->argc] = NULL;

    return PARSE_OK;
}