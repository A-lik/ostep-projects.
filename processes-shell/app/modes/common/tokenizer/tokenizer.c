//
// Created by albert.terlikpayev on 17.10.2026.
//

#include "tokenizer.h"

#include <stddef.h>
#include <string.h>

int tokenize(char *s, char **out, int max) {
    int n = 0;
    char *tok;

    while ((tok = strsep(&s, SPACE_DELIM)) != NULL) {
        if (*tok == '\0') continue;
        if (n < max) out[n] = tok;
        n++;
    }

    return n;
}
