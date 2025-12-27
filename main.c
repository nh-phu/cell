#include "headers.h"

int main() {
    char *input;
    struct cmd *commands;
    int (*pipes)[2];
    int status = 1;

    // run
    while (status) {
        printf(">");
        input = get_input();
        if (input[0] == '\n') {
            free(input);
            continue;
        }
        commands = parse_input(input, &pipes);
        status = command_handler(commands, pipes);

        free(input);
        free_commands(commands);
        free(pipes);
    }

    return 0;
}

void free_commands(struct cmd *commands) {
    if (!commands)
        return;

    for (int i = 0; commands[i].argc > 0; i++) {

        free(commands[i].args);
    }

    free(commands);
}
