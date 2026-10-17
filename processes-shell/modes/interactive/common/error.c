//
// Created by albert.terlikpayev on 16.10.2026.
//

#include "error.h"

#include <string.h>
#include <unistd.h>

void print_error() {
    const char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}
