#include "headers.h"

char *get_input() {
    size_t size = 0;
    char *input = NULL;

    // Error checking stuff

    getline(&input, &size, stdin);
    return input;
}

char **tokenize(char *input, char *separator) {
    int index = 0;
    int size = 64;
    char **tokens = (char **)malloc(size * sizeof(char *));
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

void init_cmd(struct cmd *command) {
    command->args_size = 8;
    command->argc = 0;
    command->input = STDIN;
    command->output = STDOUT;
    command->args = calloc(command->args_size, sizeof(char *));
    if (!command->args)
        exit(EXIT_FAILURE);
}

void add_arg(struct cmd *command, char *arg) {
    if (command->argc >= command->args_size) {
        command->args_size *= 2;
        command->args =
            realloc(command->args, command->args_size * sizeof(char *));
        if (!command->args)
            exit(EXIT_FAILURE);
    }
    command->args[command->argc++] = arg;
}

struct cmd *parse_input(char *input, int (**pipes)[2]) {
    char **tokens = tokenize(input, " \t\n");
    int cmd_index = 0;
    int commands_size = 36;
    struct cmd *commands = calloc(commands_size, sizeof(struct cmd));
    if (!commands)
        exit(EXIT_FAILURE);

    init_cmd(&commands[cmd_index]);
    for (int i = 0; tokens[i]; i++) {
        if (strcmp(tokens[i], "|") == 0) {
            commands[cmd_index].output = NEXT_PIPE;
            cmd_index++;
            init_cmd(&commands[cmd_index]);
            commands[cmd_index].input = PREV_PIPE;
        } else if (strcmp(tokens[i], ";") == 0) {
            cmd_index++;
            init_cmd(&commands[cmd_index]);
        } else if (strcmp(tokens[i], ">>") == 0) {
            commands[cmd_index].output = APPEND_FILE_OUT;
            cmd_index++;
            init_cmd(&commands[cmd_index]);
            commands[cmd_index].input = PREV_CMD;
            commands[cmd_index].output = END;
            add_arg(&commands[cmd_index], tokens[i + 1]);
            break;
        } else if (strcmp(tokens[i], ">") == 0) {
            commands[cmd_index].output = FILE_OUT;
            cmd_index++;
            init_cmd(&commands[cmd_index]);
            commands[cmd_index].input = PREV_CMD;
            commands[cmd_index].output = END;
            add_arg(&commands[cmd_index], tokens[i + 1]);
            break;
        } else if (strcmp(tokens[i], "<") == 0) {
            commands[cmd_index].input = FILE_IN;
            cmd_index++;
            init_cmd(&commands[cmd_index]);
            commands[cmd_index].output = PREV_CMD;
            commands[cmd_index].input = END;
            add_arg(&commands[cmd_index], tokens[i + 1]);
            break;
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

    *pipes = malloc(((cmd_index + 1) * (sizeof(int[2]))));
    if (!pipes)
        exit(EXIT_FAILURE);

    free(tokens);
    return commands;
}
