#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <unistd.h>
#include "modes/batch/batch.h"
#include "modes/interactive/interactive.h"

typedef enum { MODE_BATCH, MODE_INTERACTIVE } sh_mode_t;

static sh_mode_t choose_mode(int argc);

int main(const int argc, char *argv[]) {
    if (argc > 2) exit(0);
    const sh_mode_t mode = choose_mode(argc);

    char *prog_name = argv[0];
    strsep(&prog_name, "/");

    switch (mode) {
        case MODE_BATCH:
            batch();
            break;
        case MODE_INTERACTIVE:
            interactive(prog_name);
            break;
    }

    return 0;
}

static sh_mode_t choose_mode(const int argc) {
    sh_mode_t mode;
    if (argc == 2) mode = MODE_BATCH;
    else mode = MODE_INTERACTIVE;

    return mode;
}
