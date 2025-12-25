#include "../headers.h"

// Debug function to print parsed commands
void print_commands(struct cmd *commands) {
    int cmd_idx = 0;
    while (commands[cmd_idx].argc > 0) {
        printf("Command %d:\n", cmd_idx);
        printf("  argc: %d\n", commands[cmd_idx].argc);
        printf("  args: ");
        for (int i = 0; i < commands[cmd_idx].argc; i++) {
            printf("[%s] ", commands[cmd_idx].args[i]);
        }
        printf("\n");
        printf("  input: %d\n", commands[cmd_idx].input);
        printf("  output: %d\n", commands[cmd_idx].output);
        printf("\n");
        cmd_idx++;
    }
    printf("Total commands: %d\n", cmd_idx);
}

void test_case(char *description, char *input_str) {
    printf("=== %s ===\n", description);
    printf("Input: \"%s\"\n", input_str);
    int (*pipes)[2];

    char *input = strdup(input_str);
    struct cmd *commands = parse_input(input, &pipes);
    print_commands(commands);

    // Free memory
    free(input);
    // TODO: properly free commands array

    printf("\n");
}

int main() {
    char *input;
    int (*pipes)[2];

    input = get_input();
    struct cmd *commands = parse_input(input, &pipes);
    print_commands(commands);

    // Free memory
    free(input);
}
