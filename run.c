#include "headers.h"
#include <fcntl.h>

#define PERMISSIONS 0644

int command_handler(struct cmd *commands, int (*pipes)[2])
{
    int status = 1;
    if (strcmp(commands[0].args[0], "exit") == 0) {
        return 0;
    }

    for (int i = 0; commands[i].argc > 0; i++) {
        if (pipe(pipes[i]) == -1) {
            status = -1;
            perror("pipe failed bro lmao, try again");
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

            //  input redirection
            switch (commands[i].input) {
            case PREV_PIPE:
                dup2(pipes[i - 1][0], 0);
                close(pipes[i - 1][0]);
                break;
            case FILE_IN:
                if (commands[i + 1].args[0] == NULL) {
                    perror("No input redirection");
                } else {
                    char *source = commands[i + 1].args[0];
                    int fd;

                    fd = open(source, O_RDONLY, PERMISSIONS);
                    if (fd == -1) {
                        perror("Failed to open input file");
                    } else {
                        if (dup2(fd, 0) == -1) {
                            perror("dup2 failed for output file");
                            exit(EXIT_FAILURE);
                        }
                        close(fd);
                    }
                }
                break;
            default: {
            }
            }

            //  output redirection
            switch (commands[i].output) {
            case NEXT_PIPE:
                dup2(pipes[i][1], 1);
                break;
            case FILE_OUT:
                if (commands[i + 1].args[0] == NULL) {
                    perror("No output redirection");
                } else {
                    char *dest = commands[i + 1].args[0];
                    int fd;

                    fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, PERMISSIONS);
                    if (fd == -1) {
                        perror("Failed to open output file");
                    } else {
                        if (dup2(fd, 1) == -1) {
                            perror("dup2 failed for output file");
                            exit(EXIT_FAILURE);
                        }
                        close(fd);
                    }
                }
                break;
            case APPEND_FILE_OUT:
                if (commands[i + 1].args[0] == NULL) {
                    perror("No output redirection");
                } else {
                    char *dest = commands[i + 1].args[0];
                    int fd;

                    fd = open(dest, O_WRONLY | O_CREAT | O_APPEND, PERMISSIONS);
                    if (fd == -1) {
                        perror("Failed to open output file");
                    } else {
                        if (dup2(fd, 1) == -1) {
                            perror("dup2 failed for output file");
                            exit(EXIT_FAILURE);
                        }
                        close(fd);
                    }
                }
                break;
            default: {
            }
            }

            close(pipes[i][0]);
            close(pipes[i][1]);

            if (commands[i].input == END || commands[i].output == END) {
                return 0;
            }

            if (execvp(commands[i].args[0], commands[i].args) == -1) {
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
        if (!WIFEXITED(commands[i].wstatus) &&
            !WIFSIGNALED(commands[i].wstatus)) {
            perror("waitpid failed");
            status = -1;
            return status;
        }
    }
    return status;
}
