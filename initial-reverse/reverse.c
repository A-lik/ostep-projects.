#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "reverse.h"
#include "utils/fprinterr_exit.h"
#include "utils/linked_list.h"

static void handle_input_file(const char *src_file);
static void handle_input_output_files(const char *src_file, const char *dst_file);
static void copy_reversed_stream(FILE *src, FILE *dst);

int main(const int argc, char *argv[]) {
    if (argc > 3) {
        fprinterr_exit("usage: reverse <input> <output>");
    }

    if (argc == 2) {
        const char *input_file = argv[1];
        handle_input_file(input_file);
    }

    if (argc == 3) {
        const char *input_file = argv[1];
        const char *output_file = argv[2];
        handle_input_output_files(input_file, output_file);
    }

    if (argc == 1) {
        copy_reversed_stream(stdin, stdout);
    }

    return 0;
}

static void handle_input_file(const char *src_file) {
    FILE *src_fd = fopen(src_file, "r");

    if (!src_fd) {
        fprinterr_exit("reverse: cannot open file '%s'", src_file);
    }

    copy_reversed_stream(src_fd, stdout);
    fclose(src_fd);
}

static void handle_input_output_files(const char *src_file, const char *dst_file) {
    struct stat a, b;
    if (stat(src_file, &a) == 0 && stat(dst_file, &b) == 0 &&
        a.st_dev == b.st_dev && a.st_ino == b.st_ino) {
        fprinterr_exit("reverse: input and output file must differ");
        }

    FILE *src_fd = fopen(src_file, "r");
    if (!src_fd) {
        fprinterr_exit("reverse: cannot open file '%s'", src_file);
    }

    FILE *dst_fd = fopen(dst_file, "w");
    if (!dst_fd) {
        fprinterr_exit("reverse: cannot open file '%s'", dst_fd);
    }

    copy_reversed_stream(src_fd, dst_fd);
    fclose(src_fd);
    fclose(dst_fd);
}

static void copy_reversed_stream(FILE *src, FILE *dst) {
    char *line = NULL;
    size_t size = 0;
    struct List list;
    init_list(&list);

    while (getline(&line, &size, src) != -1) {
        push_list(&list, line);
        line = NULL;
        size = 0;
    }
    print_list(&list, dst);

    free_list(&list);
    free(line);
}