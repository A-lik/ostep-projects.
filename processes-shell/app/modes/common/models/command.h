//
// Created by albert.terlikpayev on 16.10.2026.
//

#ifndef PROCESSES_SHELL_COMMAND_H
#define PROCESSES_SHELL_COMMAND_H

// in C enums are just constants(e.g. 0,1,2,3), CMD_COUNT represents the total number of builtin commands
typedef enum { CMD_EXIT, CMD_CD, CMD_PATH, CMD_COUNT } builtin_cmd_t;

typedef struct {
    char *argv[64];
    int argc;
    char path[512];
    int is_redirect;
    char *redirect_fName;
    builtin_cmd_t builtin_cmd_num;
} command_t;

typedef struct {
    command_t parallel_cmds [64];
    int cmd_num;
} commands_t;
#endif //PROCESSES_SHELL_COMMAND_H
