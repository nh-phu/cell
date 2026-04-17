#include "../headers.h"

int builtin_exit(struct cmd *command)
{
    (void)command;
    exit(0);
}
