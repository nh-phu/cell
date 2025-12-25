#ifndef __MAIN_H__
#define __MAIN_H__

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

struct cmd {
    char **args;
    int argc;
    int args_size;
    
    int *input;
    int *output;

    pid_t pid, wpid;
    int wstatus;
};

char *get_input();
char **tokenize(char *input, char *seperator);
struct cmd *parse_input(char *input);
int command_handler(struct cmd *commands);
void exec_command(struct cmd *commands);

#endif
