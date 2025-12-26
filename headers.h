#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

typedef enum {
    STDIN,
    FILE_IN,
    PREV_PIPE,

    STDOUT,
    FILE_OUT,
    APPEND_FILE_OUT,
    NEXT_PIPE,

    PREV_CMD,
    END
} io_type;

struct cmd {
    char **args;
    int argc;
    int args_size;

    io_type input;
    io_type output;

    pid_t pid, wpid;
    int wstatus;
};

char *get_input();
char **tokenize(char *input, char *seperator);
struct cmd *parse_input(char *input, int (**pipes)[2]);
void init_cmd(struct cmd *command);
void add_arg(struct cmd *command, char *arg);
int command_handler(struct cmd *commands, int (*pipes)[2]);
void exec_command(struct cmd *commands);

#endif
