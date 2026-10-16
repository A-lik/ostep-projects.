//
// Created by albert.terlikpayev on 16.10.2026.
//

#include "modes/interactive/externalcmd/external_cmd.h"
#include "modes/interactive/command.h"
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>

static void print_error() {
    const char error_message[30] = "An error has occurred\n";
    write(STDERR_FILENO, error_message, strlen(error_message));
}

static int is_path_accessible(const command_t *cmd) {
    if (access(cmd->path, X_OK) < 0) {
        print_error();
        return 0;
    }
    return 1;
}

static int get_path(command_t *cmd) {
    snprintf(cmd->path, sizeof cmd->path, "/bin/%s", cmd->argv[0]);
    return is_path_accessible(cmd);
}

void exec_external_cmd(command_t *cmd) {
    if (get_path(cmd) == 0) return;

    pid_t pid = fork();
    if (pid == 0) {
        execv(cmd->path, cmd->argv);
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
    } else {
        printf("Error forking\n");
        exit(1);
    }
}