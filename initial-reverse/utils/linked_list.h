//
// Created by albert.terlikpayev on 05.09.2026.
//
#include <stdio.h>

#ifndef INITIAL_REVERSE_LINKED_LIST_H
#define INITIAL_REVERSE_LINKED_LIST_H
struct Node {
    char *line;
    struct Node *next;
};

struct List {
    struct Node *head;
};

void init_list(struct List *list);
void push_list(struct List *list, char *line);
void print_list(struct List *list, FILE *dst);
void free_list(struct List *list);
#endif //INITIAL_REVERSE_LINKED_LIST_H
