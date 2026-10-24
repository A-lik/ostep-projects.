//
// Created by albert.terlikpayev on 18.10.2026.
//

#include "line_parser.h"

#include <stdio.h>
#include <_string.h>

#include "app/modes/common/tokenizer/tokenizer.h"
#include "app/modes/common/models/command.h"

#define REDIRECT_DELIMITER ">"

static parse_result_t parse_redir(char *redir, command_t *cmd);

parse_result_t parse_line(char *line, commands_t *cmds) {
    cmds->cmd_num = 0;
    char *rest = line;
    char *cmd_segment;

    while ((cmd_segment = strsep(&rest, "&"))) {
        if (cmds->cmd_num >= 64) return PARSE_ERR;
        command_t *cmd = &cmds->parallel_cmds[cmds->cmd_num];
        *cmd = (command_t){0};

        char *redir = strchr(cmd_segment, '>');
        parse_result_t parse_res;
        if (redir) {
            parse_res = parse_redir(redir, cmd);
            if (parse_res != PARSE_OK) return parse_res;
        }

        int maxArgc = (int)(sizeof cmd->argv / sizeof cmd->argv[0]) - 1;
        cmd->argc = tokenize(cmd_segment, cmd->argv, maxArgc);
        if (cmd->argc > maxArgc) return PARSE_ERR;
        if (cmd->argc == 0) {
            if (redir) return PARSE_ERR;
            continue;
        }
        cmd->argv[cmd->argc] = NULL;

        cmds->cmd_num++;
    }

    return cmds->cmd_num == 0 ? PARSE_EMPTY : PARSE_OK;
}

static parse_result_t parse_redir(char *redir, command_t *cmd) {
    if (strchr(redir + 1, '>')) return PARSE_ERR;

    *redir = '\0';
    char *files[2];
    const int file_num = tokenize(redir + 1, files, 2);
    if (file_num != 1) return PARSE_ERR;

    cmd->is_redirect = 1;
    cmd->redirect_fName = files[0];

    return PARSE_OK;
}
