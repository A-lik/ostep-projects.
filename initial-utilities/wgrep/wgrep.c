#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void printerr_exit(char* err) {
    printf("%s\n", err);
    exit(1);
}

void grep_stream(FILE* f, const char* needle) {
    char* line = NULL;
    size_t size = 0;

    while (getline(&line, &size, f) != -1) {
        if (strstr(line, needle) != NULL) {
            printf("%s", line);
        }
    }

    free(line);
}

int main(int argc, char* argv[]) {
    const char* searchItem = argv[1];

    if (argc == 1) {
        printerr_exit("wgrep: searchterm [file ...]");
    }

    if (argc == 2) {
        grep_stream(stdin, searchItem);
        return 0;
    }

    for (int i = 2; i < argc; i++) {
        FILE* f = fopen(argv[i], "r");
        if (f == NULL) {
            printerr_exit("wgrep: cannot open file");
        }

        grep_stream(f, searchItem);
        fclose(f);
    }
    return 0;
}