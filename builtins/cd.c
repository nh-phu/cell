#include "../headers.h"

int cd(struct cmd *command)
{
    if (command->argc > 2 || command->argc == 0) {
        fprintf(stderr, "cd: unexpexted argument\n");
        return -1;
    }

    if (command->argc == 1 || (command->argc == 2 && strcmp(command->args[1], "~") == 0)) {
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "cd: HOME environment variable not set\n");
            return -1;
        }
        if (chdir(home) != 0) {
            perror("cd");
            return -1;
        }
        return 0;
    }

    if (chdir(command->args[1]) != 0) {
        perror("cd");
        return -1;
    }
    return 0;
}
