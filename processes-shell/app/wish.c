#include <strings.h>
#include <stdlib.h>
#include "modes/batch/batch.h"
#include "modes/interactive/interactive.h"
#include "app/path.h"
#include "modes/common/error/error.h"

typedef enum { MODE_BATCH, MODE_INTERACTIVE } sh_mode_t;

static sh_mode_t choose_mode(int argc);
static void init_path();

int main(const int argc, char *argv[]) {
    if (argc > 2) {
        print_error();
        exit(1);
    }
    init_path();
    const sh_mode_t mode = choose_mode(argc);

    char *prog_name = argv[0];
    strsep(&prog_name, "/");

    switch (mode) {
        case MODE_BATCH:
            batch(argv[1]);
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

static void init_path() {
    search_path[0] = strdup("/bin");
    search_path_len = 1;
}
