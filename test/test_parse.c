#include "../headers.h"

#include "../lexer.h"

static void print_tokens(struct token *t)
{
    for (int i = 0; t && t[i].type != TOK_END; i++) {
        switch (t[i].type) {
        case TOK_WORD:
            printf("TOK_WORD(%s)\n", t[i].text);
            break;
        case TOK_PIPE:
            puts("TOK_PIPE");
            break;
        case TOK_SEMI:
            puts("TOK_SEMI");
            break;
        case TOK_IN:
            puts("TOK_IN");
            break;
        case TOK_OUT:
            puts("TOK_OUT");
            break;
        case TOK_OUT_APP:
            puts("TOK_OUT_APP");
            break;
        case TOK_ERROR:
            puts("TOK_ERROR");
            return;
        default:
            puts("TOK_?");
            break;
        }
    }
}

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
    struct token *t = lex(input);
    print_tokens(t);
    free_tokens(t);
    struct cmd *commands = parse_input(input);
    print_commands(commands);

    // Free memory
    free(input);
    free_commands(commands);

    printf("\n");
}

int main()
{
    // Read and parse lines until EOF.
    while (1) {
        char *input = get_input();
        if (!input)
            break;

        printf("--- INPUT: %s", input);

        // Print tokens using a copy, since lex() mutates the buffer.
        char *copy = strdup(input);
        struct token *t = lex(copy);
        print_tokens(t);
        free_tokens(t);
        free(copy);

        struct cmd *commands = parse_input(input);
        if (commands) {
            print_commands(commands);
            free_commands(commands);
        }

        free(input);
    }

    return 0;
}
