#include "../headers.h"

static int update_pwd_env(const char *oldpwd)
{
    char *pwd = getcwd(NULL, 0);
    if (!pwd) {
        perror("cd: getcwd");
        return -1;
    }

    if (oldpwd) {
        if (setenv("OLDPWD", oldpwd, 1) != 0)
            perror("cd: setenv(OLDPWD)");
    }
    if (setenv("PWD", pwd, 1) != 0)
        perror("cd: setenv(PWD)");

    free(pwd);
    return 0;
}

int cd(struct cmd *command)
{
    if (command->argc > 2 || command->argc == 0) {
        fprintf(stderr, "cd: unexpexted argument\n");
        return -1;
    }

    char *oldpwd = getcwd(NULL, 0);
    if (!oldpwd) {
        perror("cd: getcwd");
        return -1;
    }

    if (command->argc == 1 ||
        (command->argc == 2 && strcmp(command->args[1], "~") == 0)) {
        char *home = getenv("HOME");
        if (home == NULL) {
            fprintf(stderr, "cd: HOME environment variable not set\n");
            free(oldpwd);
            return -1;
        }
        if (chdir(home) != 0) {
            perror("cd");
            free(oldpwd);
            return -1;
        }
        (void)update_pwd_env(oldpwd);
        free(oldpwd);
        return 0;
    }

    if (strcmp(command->args[1], "-") == 0) {
        char *target = getenv("OLDPWD");
        if (!target || target[0] == '\0') {
            fprintf(stderr, "cd: OLDPWD not set\n");
            free(oldpwd);
            return -1;
        }
        if (chdir(target) != 0) {
            perror("cd");
            free(oldpwd);
            return -1;
        }

        {
            char *pwd = getcwd(NULL, 0);
            if (pwd) {
                puts(pwd);
                free(pwd);
            }
        }
        (void)update_pwd_env(oldpwd);
        free(oldpwd);
        return 0;
    }

    if (chdir(command->args[1]) != 0) {
        perror("cd");
        free(oldpwd);
        return -1;
    }

    (void)update_pwd_env(oldpwd);
    free(oldpwd);
    return 0;
}
