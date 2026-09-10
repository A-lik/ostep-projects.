//
// Created by albert.terlikpayev on 07.09.2026.
//
#include <stdlib.h>

#ifndef INITIAL_KV_HASH_H
#define INITIAL_KV_HASH_H


typedef struct HashTable HashTable_t;

struct HashTable *ht_create(size_t capacity);
void ht_free(struct HashTable *table);
const char *ht_get_value(const struct HashTable *table, const int key);
int ht_put(struct HashTable *table, const int key, const char *value);

#endif //INITIAL_KV_HASH_H
