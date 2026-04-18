#include "headers.h"

#include <fcntl.h>

#define PERMISSIONS 0644

static int open_in(const char *path)
{
    int fd = open(path, O_RDONLY);
    if (fd == -1)
        perror("open input");
    return fd;
}

static int open_out(const char *path, int append)
{
    int flags = O_WRONLY | O_CREAT | (append ? O_APPEND : O_TRUNC);
    int fd = open(path, flags, PERMISSIONS);
    if (fd == -1)
        perror("open output");
    return fd;
}

int command_handler(struct cmd *commands)
{
    // For now we only run the first pipeline on the line.
    int n = 0;
    while (commands[n].argc > 0) {
        n++;
        if (!commands[n - 1].pipe_to_next)
            break;
    }
    if (n == 0)
        return 1;

    // Enforce the "simple" redirection rules.
    for (int i = 1; i < n; i++) {
        if (commands[i].in_path) {
            fprintf(stderr,
                    "input redirection only supported on first command\n");
            return -1;
        }
    }
    for (int i = 0; i < n - 1; i++) {
        if (commands[i].out_path) {
            fprintf(stderr,
                    "output redirection only supported on last command\n");
            return -1;
        }
    }

    const struct builtin *b0 = builtin_lookup(commands[0].args[0]);
    if (b0 && b0->parent_only && n == 1) {
        // Parent-only builtin: must run in the shell process to persist.
        if (commands[0].args[0] && strcmp(commands[0].args[0], "exit") == 0) {
            (void)b0->fn(&commands[0]);
            return 0;
        }
        return b0->fn(&commands[0]) == 0 ? 1 : -1;
    }

    int pipes[n > 1 ? n - 1 : 1][2];

    for (int i = 0; i < n; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe failed bro lmao, try again");
            return -1;
        }
        commands[i].pid = fork();
        if (commands[i].pid == -1) {
            perror("fork failed");
            return -1;
        }

        if (commands[i].pid == 0) {
            // Child: setup stdin
            if (commands[i].in_path) {
                int fd = open_in(commands[i].in_path);
                if (fd == -1)
                    exit(EXIT_FAILURE);
                if (dup2(fd, STDIN_FILENO) == -1) {
                    perror("dup2 stdin");
                    exit(EXIT_FAILURE);
                }
                close(fd);
            } else if (i > 0) {
                if (dup2(pipes[i - 1][0], STDIN_FILENO) == -1) {
                    perror("dup2 stdin pipe");
                    exit(EXIT_FAILURE);
                } else {
                    close(pipes[i - 1][0]);
                }
            }

            // Child: setup stdout
            if (commands[i].out_path) {
                int fd = open_out(commands[i].out_path, commands[i].out_append);
                if (fd == -1)
                    exit(EXIT_FAILURE);
                if (dup2(fd, STDOUT_FILENO) == -1) {
                    perror("dup2 stdout");
                    exit(EXIT_FAILURE);
                }
                close(fd);
            } else if (i < n - 1) {
                if (dup2(pipes[i][1], STDOUT_FILENO) == -1) {
                    perror("dup2 stdout pipe");
                    exit(EXIT_FAILURE);
                }
            }

            close(pipes[i][0]);
            close(pipes[i][1]);

            const struct builtin *b = builtin_lookup(commands[i].args[0]);
            if (b) {
                exit(b->fn(&commands[i]) == 0 ? EXIT_SUCCESS : EXIT_FAILURE);
            }

            execvp(commands[i].args[0], commands[i].args);
            perror("execvp");
            exit(EXIT_FAILURE);
        }

        // Parent: close fds.
        if (i > 0 && commands[i - 1].pipe_to_next) {
            close(pipes[i - 1][0]);
        }
        if (!commands[i].pipe_to_next) {
            close(pipes[i][0]);
        }
        close(pipes[i][1]);
    }

    int status = 1;
    for (int i = 0; i < n; i++) {
        commands[i].wpid = waitpid(commands[i].pid, &commands[i].wstatus, 0);
        if (commands[i].wpid == -1) {
            perror("waitpid");
            status = -1;
        }
    }

    return status;
}
