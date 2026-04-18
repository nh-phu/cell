#include "../headers.h"

// Debug function to print parsed commands
void print_commands(struct cmd *commands)
{
    int cmd_idx = 0;
    while (commands[cmd_idx].argc > 0) {
        printf("Command %d:\n", cmd_idx);
        printf("  argc: %d\n", commands[cmd_idx].argc);
        printf("  args: ");
        for (int i = 0; i < commands[cmd_idx].argc; i++) {
            printf("[%s] ", commands[cmd_idx].args[i]);
        }
        printf("\n");
        printf("  in_path: %s\n", commands[cmd_idx].in_path
                                      ? commands[cmd_idx].in_path
                                      : "(none)");
        printf("  out_path: %s\n", commands[cmd_idx].out_path
                                       ? commands[cmd_idx].out_path
                                       : "(none)");
        printf("  out_append: %d\n", commands[cmd_idx].out_append);
        printf("  pipe_to_next: %d\n", commands[cmd_idx].pipe_to_next);
        printf("\n");
        cmd_idx++;
    }
    printf("Total commands: %d\n", cmd_idx);
}

void test_case(char *description, char *input_str)
{
    printf("=== %s ===\n", description);
    printf("Input: \"%s\"\n", input_str);

    char *input = strdup(input_str);
    struct cmd *commands = parse_input(input);
    print_commands(commands);

    // Free memory
    free(input);
    free_commands(commands);

    printf("\n");
}

int main()
{
    char *input;

    input = get_input();
    struct cmd *commands = parse_input(input);
    print_commands(commands);

    // Free memory
    free(input);
    free_commands(commands);
}
