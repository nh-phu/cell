#include "../headers.h"

int cd(struct cmd *command);
int builtin_exit(struct cmd *command);

static const struct builtin BUILTINS[] = {
    {"cd", cd, 1},
    {"exit", builtin_exit, 1},
    {NULL, NULL, 0},
};

const struct builtin *builtin_lookup(const char *name)
{
    if (!name)
        return NULL;

    for (int i = 0; BUILTINS[i].name; i++) {
        if (strcmp(BUILTINS[i].name, name) == 0)
            return &BUILTINS[i];
    }
    return NULL;
}
