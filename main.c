#include "headers.h"

int main() {
    char *input;
    struct cmd *commands;
    int status;

    // run
    do {
        printf(">");
        input = get_input();
        commands = parse_input(input);
        status = command_handler(commands);
    } while (status);

    free(input);
    free(commands);
    return 0;
}
