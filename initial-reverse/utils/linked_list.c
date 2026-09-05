//
// Created by albert.terlikpayev on 05.09.2026.
//
#include "linked_list.h"
#include "fprinterr_exit.h"
#include <stdlib.h>

void init_list(struct List *list) {
    list->head = NULL;
}

// allocate memory for new node, change data of new node,
// link new node to the head(NULL for the first element), make head newly created node for the next element
void push_list(struct List *list, char *line) {
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

void free_list(struct List *list) {
    struct Node *current = list->head;
    while (current) {
        struct Node *next = current->next;
        free(current->line);
        free(current);
        current = next;
    }
    list->head = NULL;
}
