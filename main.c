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
            continue;
        }
        commands = parse_input(input, &pipes);
        status = command_handler(commands, pipes);
    }

    free(input);
    free(commands);
    free(pipes);
    return 0;
}
