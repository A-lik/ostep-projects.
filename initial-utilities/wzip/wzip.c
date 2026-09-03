#include <stdio.h>
#include "printerr_exit.h"

struct State {
    char current_char;
    int count;
};

static void reset_state(struct State *state, const char new_char) {
    state->current_char = new_char;
    state->count = 1;
}

static void emit(struct State *state) {
    fwrite(&state->count, sizeof(int), 1, stdout);
    fwrite(&state->current_char, sizeof(char), 1, stdout);
}

int main(int argc, char *argv[]) {
    char buf[1024];
    size_t n;
    struct State state = {.count = 0, .current_char = 0};

    if (argc == 1) {
        printerr_exit("wzip: file1 [file2 ...]");
    }

    for (int i = 1; i < argc; i++) {
        FILE *fp = fopen(argv[i], "r");

        if (!fp) {
            printerr_exit("wzip: cannot open file");
        }

        while ((n = fread(buf, 1, sizeof(buf), fp)) > 0) {
            for (size_t j = 0; j < n; j++) {
                if (state.count == 0) {
                    reset_state(&state, buf[j]);
                } else if (state.current_char == buf[j]) {
                    state.count++;
                } else {
                    emit(&state);
                    reset_state(&state, buf[j]);
                }
            }
        }
    }
    if (state.count > 0) {
        emit(&state);
    }
    return 0;
}