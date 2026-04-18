#include "headers.h"

int main()
{
    char *input;
    struct cmd *commands;
    int status = 1;

    // run
    while (status) {
        prompt();
        input = get_input();
        if (input[0] == '\n') {
            free(input);
            continue;
        }
        commands = parse_input(input);
        status = command_handler(commands);

        free(input);
        free_commands(commands);
    }

    return 0;
}

void free_commands(struct cmd *commands)
{
    if (!commands)
        return;

    for (int i = 0; commands[i].argc > 0; i++) {

        free(commands[i].args);
    }

    free(commands);
}
