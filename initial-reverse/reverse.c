#include "reverse.h"
#include "fprinterr_exit.h"
#include <stdio.h>
#include <stdlib.h>

static void handle_src_only(const char *src_file);
static void handle_src_dst(const char *src_file, const char *dst_file);
static void copy_stream(FILE *src, FILE *dst);

int main(const int argc, char *argv[]) {
    if (argc > 3) {
        fprinterr_exit("usage: reverse <input> <output>");
    }

    if (argc == 2) {
        const char *src_file = argv[1];
        handle_src_only(src_file);
    }

    if (argc == 3) {
        const char *input_file = argv[1];
        const char *output_file = argv[2];
        handle_src_dst(input_file, output_file);
    }

    return 0;
}

static void handle_src_only(const char *src_file) {
    FILE *src_fd = fopen(src_file, "r");

    if (!src_fd) {
        fprinterr_exit("error: cannot open file 'input.txt'");
    }

    copy_stream(src_fd, stdout);
    fclose(src_fd);
}

static void handle_src_dst(const char *src_file, const char *dst_file) {
    FILE *src_fd = fopen(src_file, "r");
    if (!src_fd) {
        fprinterr_exit("error: cannot open file 'input.txt'");
    }

    FILE *dst_fd = fopen(dst_file, "w");
    if (!src_fd) {
        fprinterr_exit("error: cannot open file 'output.txt'");
    }

    copy_stream(src_fd, dst_fd);
    fclose(src_fd);
    fclose(dst_fd);
}

static void copy_stream(FILE *src, FILE *dst) {
    char *line = NULL;
    size_t size = 0;

    while (getline(&line, &size, src) != -1) {
        fprintf(dst, "%s", line);
    }

    free(line);
}