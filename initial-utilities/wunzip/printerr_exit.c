//
// Created by albert.terlikpayev on 30.08.2026.
//

#include "printerr_exit.h"
#include <stdio.h>
#include <stdlib.h>

void printerr_exit(const char* err) {
    printf("%s\n", err);
    exit(1);
}