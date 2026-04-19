#include "headers.h"

void free_commands(struct cmd *commands)
{
    for (int i = 0; commands[i].argc > 0; i++) {
        free(commands[i].args);
    }
    free(commands);
}
