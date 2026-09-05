//
// Created by albert.terlikpayev on 30.08.2026.
//

#include "fprinterr_exit.h"
#include <stdio.h>
#include <stdlib.h>

void fprinterr_exit(const char* err) {
    fprintf(stderr, "%s\n", err);
    exit(1);
}