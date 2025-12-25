#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

typedef enum {
    STDIN = 0,
    STDOUT = 1, 
    FILE_IN = 2,
    FILE_OUT = 3,
    PREV_PIPE = 4,
    NEXT_PIPE = 5,
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
int command_handler(struct cmd *commands, int (*pipes)[2]);
void exec_command(struct cmd *commands);

#endif
