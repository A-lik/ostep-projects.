#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char *key, *value;
    char *sample = malloc(10);
    strcpy(sample, "10,albert");

    key = strsep(&sample, ",");
    value = strsep(&sample, ",");

    printf("key: %s\n", key);
    printf("value: %s\n", value);

    return 0;
}