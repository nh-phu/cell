#ifndef __MAIN_H__
#define __MAIN_H__

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

struct cmd {
    char *in_path;
    char *out_path;
    int out_append;

    // Pipeline control
    int pipe_to_next;

    pid_t pid, wpid;
    int wstatus;

    int argc;
    int args_size;
    char **args;
};

struct builtin {
    const char *name;
    int (*fn)(struct cmd *command);
    int parent_only;
};

char *get_input();
void prompt();
struct cmd *parse_input(char *input);
int command_handler(struct cmd *commands);

const struct builtin *builtin_lookup(const char *name);

void free_commands(struct cmd *commands);

#endif
