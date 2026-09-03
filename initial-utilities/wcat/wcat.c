#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    char buffer[1024];

    for (int i = 1; i < argc; i++) {
        char* filename = argv[i];
        FILE* fp = fopen(filename, "r");

        if (!fp) {
            printf("wcat: cannot open file\n");
            exit(1);
        }

        while (fgets(buffer, sizeof(buffer), fp) != NULL) {
            printf("%s", buffer);
        }

        fclose(fp);
    }
    return 0;
}
