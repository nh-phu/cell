#include "headers.h"
#include "lexer.h"

static int ends_with_unescaped_backslash_newline(const char *s)
{
    size_t len = strlen(s);
    if (len < 2)
        return 0;
    if (s[len - 1] != '\n' || s[len - 2] != '\\')
        return 0;

    // Count consecutive backslashes before the newline.
    size_t bs = 0;
    for (size_t i = len - 2;; i--) {
        if (s[i] != '\\')
            break;
        bs++;
        if (i == 0)
            break;
    }

    return (bs % 2) == 1;
}

char *get_input()
{
    size_t cap = 0;
    char *input = NULL;

    ssize_t nread = getline(&input, &cap, stdin);
    if (nread == -1) {
        free(input);
        return NULL;
    }

    while (input && ends_with_unescaped_backslash_newline(input)) {
        // Remove the backslash-newline and continue reading.
        size_t len = strlen(input);
        input[len - 2] = '\0';

        fputs("> ", stdout);
        fflush(stdout);

        char *next = NULL;
        size_t next_cap = 0;
        ssize_t next_read = getline(&next, &next_cap, stdin);
        if (next_read == -1) {
            free(next);
            break;
        }

        size_t a = strlen(input);
        size_t b = strlen(next);
        char *joined = realloc(input, a + b + 1);
        if (!joined) {
            free(next);
            break;
        }
        input = joined;
        memcpy(input + a, next, b + 1);
        free(next);
    }

    return input;
}

static void init_cmd(struct cmd *command)
{
    command->args_size = 8;
    command->argc = 0;
    command->in_path = NULL;
    command->out_path = NULL;
    command->out_append = 0;
    command->pipe_to_next = 0;
    command->args = calloc(command->args_size, sizeof(char *));
    if (!command->args)
        exit(EXIT_FAILURE);
}

static void add_arg(struct cmd *command, char *arg)
{
    if (command->argc >= command->args_size) {
        command->args_size *= 2;
        command->args =
            realloc(command->args, command->args_size * sizeof(char *));
        if (!command->args)
            exit(EXIT_FAILURE);
    }
    command->args[command->argc++] = arg;
}

struct cmd *parse_input(char *input)
{
    struct token *tokens = lex(input);
    if (!tokens)
        return NULL;

    int cmd_index = 0;
    int commands_size = 36;
    struct cmd *commands = calloc(commands_size, sizeof(struct cmd));
    if (!commands)
        exit(EXIT_FAILURE);

    init_cmd(&commands[cmd_index]);
    for (int i = 0; tokens[i].type != TOK_END; i++) {
        if (tokens[i].type == TOK_ERROR) {
            break;
        }

        if (tokens[i].type == TOK_PIPE) {
            if (commands[cmd_index].argc == 0) {
                fprintf(stderr, "syntax error near unexpected token '|'\n");
                break;
            }
            commands[cmd_index].pipe_to_next = 1;
            cmd_index++;
            init_cmd(&commands[cmd_index]);
        } else if (tokens[i].type == TOK_SEMI) {
            if (commands[cmd_index].argc == 0) {
                fprintf(stderr, "syntax error near unexpected token ';'\n");
                break;
            }
            // End current pipeline; start a new command.
            commands[cmd_index].pipe_to_next = 0;
            cmd_index++;
            init_cmd(&commands[cmd_index]);
        } else if (tokens[i].type == TOK_OUT_APP || tokens[i].type == TOK_OUT) {
            if (commands[cmd_index].out_path) {
                fprintf(stderr, "multiple output redirections not supported\n");
                break;
            }
            if (tokens[i + 1].type != TOK_WORD) {
                fprintf(stderr, "missing output file\n");
                break;
            }
            commands[cmd_index].out_path = tokens[i + 1].text;
            commands[cmd_index].out_append = (tokens[i].type == TOK_OUT_APP);
            i++;
        } else if (tokens[i].type == TOK_IN) {
            if (commands[cmd_index].in_path) {
                fprintf(stderr, "multiple input redirections not supported\n");
                break;
            }
            if (tokens[i + 1].type != TOK_WORD) {
                fprintf(stderr, "missing input file\n");
                break;
            }
            commands[cmd_index].in_path = tokens[i + 1].text;
            i++;
        } else if (tokens[i].type == TOK_WORD) {
            add_arg(&commands[cmd_index], tokens[i].text);
            commands[cmd_index].args[commands[cmd_index].argc] = NULL;
        } else {
            fprintf(stderr, "syntax error\n");
            break;
        }

        if (cmd_index >= commands_size) {
            commands_size *= 2;
            commands = realloc(commands, commands_size * sizeof(struct cmd));
            if (!commands)
                exit(EXIT_FAILURE);
        };
    }

    free_tokens(tokens);
    return commands;
}
