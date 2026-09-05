#include "reverse.h"
#include "fprinterr_exit.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static void handle_input_file(const char *src_file);
static void handle_input_output_files(const char *src_file, const char *dst_file);
static void copy_reversed_stream(FILE *src, FILE *dst);

struct Node {
    char *line;
    struct Node *next;
};

struct List {
    struct Node *head;
};

static void init_list(struct List *list) {
    list->head = NULL;
}

// allocate memory for new node, change data of new node,
// link new node to the head(NULL for the first element), make head newly created node for the next element
static void push_list(struct List *list, char *line) {
    struct Node *new_node = malloc(sizeof(struct Node));
    if (!new_node) {
        fprinterr_exit("malloc failed");
    }
    new_node->line = line;
    new_node->next = list->head;
    list->head = new_node;
}

void print_list(struct List *list, FILE *dst) {
    struct Node *current = list->head;
    while (current) {
        fprintf(dst, "%s", current->line);
        current = current->next;
    }
}

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

    free(line);
}