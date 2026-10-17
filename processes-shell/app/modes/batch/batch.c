#include <stdio.h>
#include <stdlib.h>

#include "app/modes/batch/batch.h"
#include "app/modes/common/cmd_parser/cmd_parser.h"
#include "app/modes/common/cmd_runner/cmd_runner.h"
#include "app/modes/common/error/error.h"
#include "app/modes/common/models/command.h"

void batch(char *file_name) {
    FILE *fp = fopen(file_name, "r");
    if (!fp) print_error();

    char *cmd_line = NULL;
    size_t cmd_cap = 0;

    while (getline(&cmd_line, &cmd_cap, fp) != -1) {
        command_t cmd = {0};
        parse_cmd(&cmd, &cmd_line);
        if (run_cmd(&cmd) < 0) print_error();
    }

    free(cmd_line);
}
