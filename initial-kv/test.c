#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "stdbool.h"
#include "hash.h"


int main() {
    struct HashTable *table = ht_create(4);

    ht_put(table, 1, "Albert1");
    ht_put(table, 2, "Terlikpayev2");
    ht_put(table, 2, "Albert2");
    ht_put(table, 3, "Terlikpayev3");
    ht_put(table, 4, "Albert4");
    ht_put(table, 5, "Albert5");
    ht_put(table, 6, "Albert6");
    ht_put(table, 7, "Albert7");
    ht_put(table, 8, "Albert8");
    ht_put(table, 9, "Albert9");
    ht_put(table, 10, "Albert10");
    ht_put(table, 17, "Albert17");


    const char *value1 = ht_get_value(table, 1);
    const char *value2 = ht_get_value(table, 2);
    const char *value3 = ht_get_value(table, 3);
    const char *value4 = ht_get_value(table, 4);
    const char *value5 = ht_get_value(table, 5);
    const char *value6 = ht_get_value(table, 6);
    const char *value7 = ht_get_value(table, 7);
    const char *value8 = ht_get_value(table, 8);
    const char *value9 = ht_get_value(table, 9);
    const char *value10 = ht_get_value(table, 10);
    const char *value17 = ht_get_value(table, 17);
    const char *value11 = ht_get_value(table, 11); // misssing one

    printf("value1: %s\n", value1);
    printf("value2: %s\n", value2);
    printf("value3: %s\n", value3);
    printf("value4: %s\n", value4);
    printf("value5: %s\n", value5);
    printf("value6: %s\n", value6);
    printf("value7: %s\n", value7);
    printf("value8: %s\n", value8);
    printf("value9: %s\n", value9);
    printf("value10: %s\n", value10);
    printf("value17: %s\n", value17);
    printf("value11: %s\n", value11);

    bool result_of_delete9 = ht_delete(table, 9);
    printf("value9 was deleted: %s\n", result_of_delete9 ? "true" : "false");
    printf("value1: %s\n", ht_get_value(table, 1));
    printf("value17: %s\n", ht_get_value(table, 17));

    bool result_of_delete17 = ht_delete(table, 17);
    printf("value17 was deleted: %s\n", result_of_delete17 ? "true" : "false");
    printf("value1: %s\n", ht_get_value(table, 1));

    bool result_of_delete1 = ht_delete(table, 1);
    printf("value1 was deleted: %s\n", result_of_delete1 ? "true" : "false");
    printf("value1: %s\n", ht_get_value(table, 1));

    ht_delete(table, 99);

    ht_clear(table);
    bool was_value2_put = ht_put(table, 2, "Albert2");
    printf("value2 was put: %s\n", was_value2_put ? "true" : "false");
    value2 = ht_get_value(table, 2);
    printf("value2: %s\n", value2);

    ht_free(table);
}