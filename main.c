#include "headers.h"

int main() {
    char *input;
    struct cmd *commands;
    int (*pipes)[2];
    int status = 1;

    // run
    do {
        printf(">");
        input = get_input();
        commands = parse_input(input, &pipes);
        status = command_handler(commands, pipes);
    } while (status);

    free(input);
    free(commands);
    free(pipes);
    return 0;
}
