#include <stdio.h>
#include "printerr_exit.h"

struct State {
    char current_char;
    int count;
};

static void emit(struct State *state) {
    for (int i = 0; i < state->count; i++) {
        printf("%c", state->current_char);
    }
}

int main(const int argc, char *argv[]) {
    if (argc == 1) {
        printerr_exit("wunzip: file1 [file2 ...]");
    }
    struct State state = {.count = 0, .current_char = 0};

    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");
        if (fp == NULL) {
            printerr_exit("wunzip: cannot open file");
        }

        while (fread(&state.count, sizeof(int), 1, fp) == 1) {
            fread(&state.current_char, sizeof(char), 1, fp);
            emit(&state);
        }
    }

    return 0;
}