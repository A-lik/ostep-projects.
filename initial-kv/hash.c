//
// Created by albert.terlikpayev on 07.09.2026.
//
#include "hash.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define HT_MIN_CAPACITY 8

struct Entry {
    int key;
    char *value;
    struct Entry *next;
};

struct HashTable {
    struct Entry **buckets;
    size_t capacity; // how many buckets the array has
    size_t size; // how many key-value pairs are actually stored
};

struct HashTable *ht_create(size_t capacity) {
    struct HashTable *table = malloc(sizeof(struct HashTable));
    if (table == NULL) {
        return NULL;
    }

    size_t cap = HT_MIN_CAPACITY;
    while (cap < capacity) {
        cap *= 2;
    }

    // printf("capacity: %lu\n", cap);

    struct Entry **buckets = calloc(cap, sizeof(struct Entry *));
    if (buckets == NULL) {
        free(table);
        return NULL;
    }

    table->buckets = buckets;
    table->capacity = cap;
    table->size = 0;

    return table;
}

void ht_free(struct HashTable *table) {
    if (table == NULL) return;
    ht_clear(table);
    free(table->buckets);
    free(table);
}

void ht_clear(struct HashTable *table) {
    if (table == NULL) return;
    for (size_t i = 0; i < table->capacity; i++) {
        struct Entry *entry = table->buckets[i];
        while (entry != NULL) {
            free(entry->value);
            struct Entry *next = entry->next;
            free(entry);
            entry = next;
        }
        table->buckets[i] = NULL;
    }
    table->size = 0;
}

bool ht_is_empty(struct HashTable *table) {
    if (table == NULL) return true;
    if (table->size == 0) return true;
    return false;
}

static size_t ht_index(const struct HashTable *table, const int key) {
    return (unsigned int)key % table->capacity;
}

const char *ht_get_value(const struct HashTable *table, const int key) {
    struct Entry *entry = table->buckets[ht_index(table, key)];
    while (entry != NULL) {
        if (entry->key == key) {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

bool ht_put(struct HashTable *table, const int key, const char *value) {
    size_t index = ht_index(table, key);
    struct Entry *entry = table->buckets[index];
    // check if the entry if the key is already exist, if true, then overwrite value with that key
    while (entry != NULL) {
        if (entry->key == key) {
            char *temp_value = strdup(value);
            if (temp_value == NULL) return false;
            free(entry->value);
            entry->value = temp_value;
            // printf("ht_put capacity: %lu\n", table->capacity);
            // printf("ht_put size: %lu\n", table->size);
            return true;
        }
        entry = entry->next;
    }

    // if entry with the provided key does not exist branch
    struct Entry *new_entry = malloc(sizeof(struct Entry));
    if (new_entry == NULL) return false;
    char *temp_value = strdup(value);
    if (temp_value == NULL) {
        free(new_entry);
        return false;
    }
    new_entry->key = key;
    new_entry->value = temp_value;
    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
    table->size++;
    // printf("ht_put capacity: %lu\n", table->capacity);
    // printf("ht_put size: %lu\n", table->size);
    return true;
}

bool ht_delete(struct HashTable *table, const int key) {
    size_t index = ht_index(table, key);
    struct Entry **ppEntry = &table->buckets[index];

    while (*ppEntry != NULL) {
        if ((*ppEntry)->key == key) {
            struct Entry *victim = *ppEntry;
            *ppEntry = victim->next;
            free(victim->value);
            free(victim);
            table->size--;
            return true;
        }
        ppEntry = &(*ppEntry)->next;
    }

    return false;
}

void ht_foreach(
    struct HashTable *table,
    void (*fn)(int key, const char *value, void *context),
    void *context) {
    for (size_t i =0; i < table->capacity; i++) {
        struct Entry *entry = table->buckets[i];
        while (entry != NULL) {
            fn(entry->key, entry-> value, context);
            entry = entry->next;
        }
    }
}
