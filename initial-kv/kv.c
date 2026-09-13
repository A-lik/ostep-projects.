#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "hash.h"
#include "entry.c"

static void do_put(struct HashTable *table, const int key, const char *value, bool *mutated);
static void do_get(struct HashTable *table, const int key);
static void do_delete(struct HashTable *table, const int key, bool *mutated);
static void do_clear(struct HashTable *table, bool *mutated);
static void do_all(struct HashTable *table);

static void load_to_ht(struct HashTable *table, char *line);
static struct Entry *to_entry(char *line, struct Entry *out);

static void print_entry(const int key, const char *value, void *context);
static void write_entry(const int key, const char *value, void *context);

int main(const int argc, char *argv[]) {
    // if (argc == 1) {
    //     printf("exited with 0 passed args\n");
    //     exit(0);
    // }

    FILE *db_file = fopen("database.txt", "r");
    char *line = NULL;
    size_t size = 0;
    struct HashTable *table = ht_create(8);
    bool mutated = false;

    if (db_file != NULL) {
        ssize_t n;
        while ((n = getline(&line, &size, db_file)) != -1) {
            if (n > 0 && line[n - 1] == '\n') line[n - 1] = '\0';
            load_to_ht(table, line);
        }
        free(line);
        fclose(db_file);
    }

    for (int i = 1; i < argc; i++) {
        char *cmd;
        int key;
        char *value;

        if (strlen(argv[i]) > 1) {
            cmd = strsep(&argv[i], ",");
            key = atoi(strsep(&argv[i], ","));
            value = strsep(&argv[i], ",");
        }
        else cmd = argv[i];

        if (strcmp(cmd, "p") == 0)      do_put(table, key, value, &mutated);
        else if (strcmp(cmd, "g") == 0) do_get(table, key);
        else if (strcmp(cmd, "d") == 0) do_delete(table, key, &mutated);
        else if (strcmp(cmd, "c") == 0) do_clear(table, &mutated);
        else if (strcmp(cmd, "a") == 0) do_all(table);
        else                            printf("bad command\n");
    }

    if (mutated) {
        FILE *temp_db_file = fopen("database_temp.txt", "w");
        ht_foreach(table, write_entry, temp_db_file);
        fclose(temp_db_file);
        rename("database_temp.txt", "database.txt");
    }

    ht_free(table);
    return 0;
}

static void do_put(struct HashTable *table, const int key, const char *value, bool *mutated) {
    bool put_result = ht_put(table, key, value);
    if (put_result) *mutated = true;
    printf(put_result ? "%d,%s saved successfully\n" : "failed to save %d,%s\n", key, value);
}

static void do_get(struct HashTable *table, const int key) {
    const char *value = ht_get_value(table, key);
    if (value == NULL) {
        printf("%d not found\n", key);
        return;
    }
    printf("%d,%s\n", key, value);
}

static void do_delete(struct HashTable *table, const int key, bool *mutated) {
    bool delete_result = ht_delete(table, key);
    if (delete_result) *mutated = true;
    printf(delete_result ? "%d deleted successfully\n" : "%d not found\n", key);
}

static void do_clear(struct HashTable *table, bool *mutated) {
    if (!ht_is_empty(table)) {
        ht_clear(table);
        *mutated = true;
    }
}

static void do_all(struct HashTable *table) {
    ht_foreach(table, print_entry, NULL);
}

static void print_entry(const int key, const char *value, void *context) {
    (void)context;
    printf("%d,%s\n", key, value);
}

static void write_entry(const int key, const char *value, void *context) {
    FILE *f = context;
    fprintf(f, "%d,%s\n", key, value);
}

static void load_to_ht(struct HashTable *table, char *line) {
    struct Entry entry;
    to_entry(line, &entry);
    // printf("%s", entry.value);
    ht_put(table, entry.key, entry.value);
}

static struct Entry *to_entry(char *line, struct Entry *out) {
    char *key = strsep(&line, ",");
    char *value = strsep(&line, ",");
    out->key = atoi(key);
    out->value = value;

    return out;
}

/*
 * 1. On program start, need to read the file and load its content into memory data structure(I chose hash table)
 * 2. Then when processing commands(put get and etc.) update in memory data structure
 * 3. Before exiting program have to write out that updated in-memory data structure to the file
 */