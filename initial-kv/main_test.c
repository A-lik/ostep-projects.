// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
//
// int main() {
//     char *a = malloc(2);
//     strcpy(a, "Hello");
//     char *c = "asdasd";
//
//     printf("%s\n", a);
//     printf("%s\n", c);
// }

#include <string.h>
#include <stdio.h>


void greet(const char *name) {
    char buf[8];              // 8 bytes on the stack
    strcpy(buf, name);        // no size check
    printf("Hi %s\n", buf);
}

int main() {
    greet("this string is definitely longer than eight bytes");
}