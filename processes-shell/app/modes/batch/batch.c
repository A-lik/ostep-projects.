#include <stdio.h>
#include <stdlib.h>

#include "app/modes/batch/batch.h"
#include "app/modes/common/cmd_runner/cmd_runner.h"
#include "app/modes/common/error/error.h"
#include "app/modes/common/models/command.h"
#include "app/modes/common/parser/input_parser/line_parser.h"

void batch(char *file_name) {
    FILE *fp = fopen(file_name, "r");
    if (!fp) {
        print_error();
        exit(1);
    }

    char *input_line = NULL;
    size_t input_cap = 0;

    while (getline(&input_line, &input_cap, fp) != -1) {
        commands_t cmds;
        switch (parse_line(input_line, &cmds)) {
            case PARSE_EMPTY: continue;
            case PARSE_OK: break;
            case PARSE_ERR: print_error(); continue;
        }
        run_cmds(&cmds);
    }

    free(input_line);
}
