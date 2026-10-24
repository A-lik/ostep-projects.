#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "app/modes/interactive/interactive.h"
#include "app/modes/common/models/command.h"
#include "app/modes/common/cmd_runner/cmd_runner.h"
#include "app/modes/common/error/error.h"
#include "app/modes/common/parser/input_parser/line_parser.h"

static void print_sh_name(char *prog_name);

void interactive(char *prog_name) {
    char *input_line = NULL;
    size_t input_cap = 0;

    while (1) {
        print_sh_name(prog_name);
        commands_t cmds;

        if (getline(&input_line, &input_cap, stdin) == -1) break;
        switch (parse_line(input_line, &cmds)) {
            case PARSE_EMPTY: continue;
            case PARSE_OK: break;
            case PARSE_ERR: print_error(); continue;
        }
        run_cmds(&cmds);
    }
    free(input_line);
}

static void print_sh_name(char *prog_name) {
    printf("%s> ", prog_name);
    fflush(stdout);
}
