//
// Created by albert.terlikpayev on 16.10.2026.
//

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "app/path.h"
#include "app/modes/common/models/command.h"

static int get_path(command_t *cmd);
static int is_path_accessible(const command_t *cmd);
static int redirect_output(char *filename);

int exec_external_cmd(command_t *cmd) {
    if (get_path(cmd) != 0) return -1;

    pid_t pid = fork();
    if (pid < 0) return -1;
    if (pid == 0) {
        if (cmd->is_redirect) redirect_output(cmd->redirect_fName);
        if (execv(cmd->path, cmd->argv) == -1) return -1;
    }

    waitpid(pid, NULL, 0);
    return 0;
}

static int get_path(command_t *cmd) {
    for (int i = 0; i < search_path_len; i++) {
        snprintf(cmd->path, sizeof cmd->path, "%s/%s", search_path[i], cmd->argv[0]);
        if (is_path_accessible(cmd)) return 0;
    }

    return -1;
}

static int is_path_accessible(const command_t *cmd) {
    if (access(cmd->path, X_OK) < 0) return 0;
    return 1;
}

static int redirect_output(char *filename) {
    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) return -1;
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);

    return 0;
}