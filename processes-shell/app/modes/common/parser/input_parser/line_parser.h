//
// Created by albert.terlikpayev on 18.10.2026.
//

#ifndef WISH_INPUT_PARSER_H
#define WISH_INPUT_PARSER_H
#include "app/modes/common/models/command.h"

typedef enum { PARSE_OK, PARSE_EMPTY, PARSE_ERR } parse_result_t;

parse_result_t parse_line(char *line, commands_t *cmds);
#endif //WISH_INPUT_PARSER_H
