//
// Created by albert.terlikpayev on 30.08.2026.
//

#include "fprinterr_exit.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

__attribute__((noreturn, format(printf, 1, 2)))
void fprinterr_exit(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    exit(1);
}