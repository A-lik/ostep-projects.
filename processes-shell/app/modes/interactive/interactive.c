#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "app/modes/interactive/interactive.h"
#include "app/modes/common/cmd_parser/cmd_parser.h"
#include "app/modes/common/models/command.h"
#include "app/modes/common/cmd_runner/cmd_runner.h"
#include "app/modes/common/error/error.h"


static void print_sh_name(char *prog_name);

void interactive(char *prog_name) {
    char *cmd_line = NULL;
    size_t cmd_cap = 0;

    while (1) {
        print_sh_name(prog_name);
        command_t cmd = {0};

        if (getline(&cmd_line, &cmd_cap, stdin) == -1) break;
        parse_cmd(&cmd, &cmd_line);
        if (run_cmd(&cmd) < 0) print_error();
    }

    free(cmd_line);
}

static void print_sh_name(char *prog_name) {
    printf("%s> ", prog_name);
    fflush(stdout);
}
