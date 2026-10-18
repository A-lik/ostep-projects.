//
// Created by albert.terlikpayev on 18.10.2026.
//

#ifndef WISH_INPUT_PARSER_H
#define WISH_INPUT_PARSER_H
#include "app/modes/common/models/command.h"

typedef enum { PARSE_OK, PARSE_EMPTY, PARSE_ERR } parse_status_t;

parse_status_t parse_line(char *line, command_t *cmd);
#endif //WISH_INPUT_PARSER_H
