#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>

struct EntryLocal {
    int key;
    char *value;
    struct EntryLocal *next;
};

struct HashTableLocal {
    struct EntryLocal **buckets;
    size_t capacity;
    size_t size;
};

int main(void) {
    struct HashTableLocal *t = malloc(sizeof *t);
    t->capacity = 2;
    t->size = 2;
    t->buckets = calloc(t->capacity, sizeof *t->buckets);

    struct EntryLocal *a = malloc(sizeof *a);
    *a = (struct EntryLocal){1, "one", NULL};
    t->buckets[1 % t->capacity] = a;

    struct EntryLocal *b = malloc(sizeof *b);
    *b = (struct EntryLocal){2, "two", NULL};
    t->buckets[2 % t->capacity] = b;

    struct EntryLocal **pp = &t->buckets[1];
    struct EntryLocal *pP = t->buckets[1];
    printf("%p\n", pP);
    printf("%p\n", pp);
    printf("%p\n", *pp);

    printf("%s %s\n", t->buckets[1]->value, t->buckets[0]->value);

    int c = 4;
    int *pc = &c;
    int **ppc = &pc;

    printf("address of pc %p\n", pc);
    printf("address of ppc %p\n", ppc);
    printf("address of pc %p\n", *ppc);
}