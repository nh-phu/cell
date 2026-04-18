#include "headers.h"

char *get_input()
{
    size_t size = 0;
    char *input = NULL;

    // Error checking stuff

    getline(&input, &size, stdin);
    return input;
}

static char **tokenize(char *input, char *separator)
{
    int index = 0;
    int size = 64;
    char **tokens = malloc(size * sizeof(char *));
    if (!tokens)
        exit(EXIT_FAILURE);

    char *token;
    char *save;
    token = strtok_r(input, separator, &save);
    while (token != NULL) {
        tokens[index++] = token;

        if (index >= size) {
            size *= 2;
            tokens = realloc(tokens, size * sizeof(char *));
            if (!tokens)
                exit(EXIT_FAILURE);
        };
        token = strtok_r(NULL, separator, &save);
    }
    tokens[index] = NULL;
    return tokens;
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
    char **tokens = tokenize(input, " \t\n");
    int cmd_index = 0;
    int commands_size = 36;
    struct cmd *commands = calloc(commands_size, sizeof(struct cmd));
    if (!commands)
        exit(EXIT_FAILURE);

    init_cmd(&commands[cmd_index]);
    for (int i = 0; tokens[i]; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            commands[cmd_index].pipe_to_next = 1;
            cmd_index++;
            init_cmd(&commands[cmd_index]);
        } else if (strcmp(tokens[i], ";") == 0) {
            // TODO: support multiple commands separated by ';'
            fprintf(stderr, "';' not supported yet\n");
            break;
        } else if (strcmp(tokens[i], ">>") == 0 ||
                   strcmp(tokens[i], ">") == 0) {
            if (commands[cmd_index].out_path) {
                fprintf(stderr, "multiple output redirections not supported\n");
                break;
            }
            if (!tokens[i + 1]) {
                fprintf(stderr, "missing output file\n");
                break;
            }
            commands[cmd_index].out_path = tokens[i + 1];
            commands[cmd_index].out_append = (strcmp(tokens[i], ">>") == 0);
            i++;
        } else if (strcmp(tokens[i], "<") == 0) {
            if (commands[cmd_index].in_path) {
                fprintf(stderr, "multiple input redirections not supported\n");
                break;
            }
            if (!tokens[i + 1]) {
                fprintf(stderr, "missing input file\n");
                break;
            }
            commands[cmd_index].in_path = tokens[i + 1];
            i++;
        } else {
            add_arg(&commands[cmd_index], tokens[i]);
            commands[cmd_index].args[commands[cmd_index].argc] = NULL;
        }

        if (cmd_index >= commands_size) {
            commands_size *= 2;
            commands = realloc(commands, commands_size * sizeof(struct cmd));
            if (!commands)
                exit(EXIT_FAILURE);
        };
    }

    free(tokens);
    return commands;
}
