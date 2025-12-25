#include "headers.h"

int command_handler(struct cmd *commands, int (*pipes)[2]) {
    int status = 1;
    if (strcmp(commands[0].args[0], "exit") == 0) {
        return 0;
    }

    for (int i = 0; commands[i].argc > 0; i++) {
        if (pipe(pipes[i]) == -1) {
            status = -1;
            perror("pipe failed bro lmao try again");
            break;
        }

        commands[i].pid = fork();
        switch (commands[i].pid) {
        case -1:
            status = -1;
            perror("fork failed");
            break;
        case 0:
            // Child process
            if (commands[i].input == PREV_PIPE) {
                dup2(pipes[i - 1][0], 0);
                close(pipes[i - 1][0]);
            } else if (commands[i].input == FILE_IN) {
                // Handle file input redirection
                // (Implementation not shown here)
            }

            if (commands[i].output == NEXT_PIPE) {
                dup2(pipes[i][1], 1);
            } else if (commands[i].output == FILE_OUT) {
                // Handle file output redirection
                // (Implementation not shown here)
            }

            close(pipes[i][0]);
            close(pipes[i][1]);

            if (execvp(commands[i].args[0], commands[i].args) == -1) {
                status = -1;
                perror("execvp failed");
            }
            exit(EXIT_FAILURE);
        default: {
            // // Parent process
            if (commands[i].input == PREV_PIPE) {
                close(pipes[i - 1][0]);
            }
            if (commands[i].output != NEXT_PIPE) {
                close(pipes[i][0]);
            }
            close(pipes[i][1]);
        }
        }
    }
    for (int i = 0; commands[i].argc > 0; i++) {
        commands[i].wpid = waitpid(commands[i].pid, &commands[i].wstatus, 0);
        if (WIFEXITED(commands[i].wstatus)) {
            int exit_status = WEXITSTATUS(commands[i].wstatus);
            if (exit_status != 0) {
                status = exit_status;
            }
        }
    }
    return status;
}
