#include "headers.h"

int command_handler(struct cmd *commands) {
    int status = 1;
    if (strcmp(commands[0].args[0], "exit") == 0) {
        return 0;
    }

    for (int i = 0; commands[i].argc > 0; i++) {
        commands[i].pid = fork();
        switch (commands[i].pid) {
        case -1:
            status = -1;
            perror("fork failed");
            break;
        case 0:
            // Child process
            if (execvp(commands[i].args[0], commands[i].args) == -1) {
                perror("execvp failed");
            }
            exit(EXIT_FAILURE);
        default: {
            // Parent process
            commands[i].wpid = waitpid(commands[i].pid, &commands[i].wstatus, 0);
        }
        }
    }
    return status;
}
