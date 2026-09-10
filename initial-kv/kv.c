#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

static void do_put(char *key, char *value, FILE *db_file_decr);
static void do_get(char *key, FILE *db_file_decr);
static void do_delete(char *key, FILE *db_file_decr);
static void do_clear(FILE *db_file_decr);
static void do_all(FILE *db_file_decr);


int main(const int argc, char *argv[]) {
    if (argc == 1) {
        printf("exited with 0 passed args\n");
        exit(0);
    }

    FILE *db_file = fopen("database.txt", "r");
    char *line = NULL;
    size_t size = 0;

    if (db_file == NULL) {
        ht_create(0);
    } else {
        while (getline(&line, &size, db_file) != -1) {
            parse(line);
        }
    }

    char *cmd;
    char *key;
    char *value;

    for (int i = 1; i < argc; i++) {
        if (strlen(argv[i]) > 1) {
            cmd = strsep(&argv[i], ",");
            key = strsep(&argv[i], ",");
            value = strsep(&argv[i], ",");
        }
        else cmd = argv[i];

        if (strcmp(cmd, "p") == 0)      do_put(key, value, db_file);
        else if (strcmp(cmd, "g") == 0) do_get(key, db_file);
        else if (strcmp(cmd, "d") == 0) do_delete(key, db_file);
        else if (strcmp(cmd, "c") == 0) do_clear(db_file);
        else if (strcmp(cmd, "a") == 0) do_all(db_file);
        else                            printf("bad command\n");
    }
    return 0;
}

static void do_put(char *key, char *value, FILE *db_file_decr) {
    fprintf(db_file_decr, "%s,%s\n", key, value);
    printf("put\n");
}

static void do_get(char *key, FILE *db_file_decr) {
    printf("get\n");
}

static void do_delete(char *key, FILE *db_file_decr) {
    printf("delete\n");
}

static void do_clear(FILE *db_file_decr) {
    printf("clear\n");
}

static void do_all(FILE *db_file_decr) {
    printf("all\n");
}

/*
 * 1. On program start, need to read the file and load its content into memory data structure(I chose hash table)
 * 2. Then when processing commands(put get and etc.) update in memory data structure
 * 3. Before exiting program have to write out that updated in-memory data structure to the file
 */