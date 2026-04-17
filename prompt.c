#include "headers.h"

#include <limits.h>

static void print_cwd()
{
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        fputs("?", stdout);
        return;
    }

    const char *home = getenv("HOME");
    if (home && home[0] != '\0') {
        size_t home_len = strlen(home);
        if (strncmp(cwd, home, home_len) == 0 &&
            (cwd[home_len] == '\0' || cwd[home_len] == '/')) {
            if (cwd[home_len] == '\0') {
                fputc('~', stdout);
                return;
            }

            const char *base = strrchr(cwd, '/');
            if (!base || base[1] == '\0') {
                fputc('~', stdout);
                return;
            }

            fputs(base + 1, stdout);
            return;
        }
    }

    // Non-HOME: show only basename; special-case root.
    if (strcmp(cwd, "/") == 0) {
        fputc('/', stdout);
        return;
    }

    const char *base = strrchr(cwd, '/');
    if (!base || base[1] == '\0') {
        fputs(cwd, stdout);
        return;
    }
    fputs(base + 1, stdout);
}

void prompt(void)
{
    const char *user = getenv("USER");
    if (!user || user[0] == '\0')
        user = "?";

    char host[256];
    if (gethostname(host, sizeof(host)) != 0)
        strcpy(host, "?");
    host[sizeof(host) - 1] = '\0';

    fputc('[', stdout);
    fputs(user, stdout);
    fputc('@', stdout);
    fputs(host, stdout);
    fputc(' ', stdout);
    print_cwd();
    fputs("]$ ", stdout);
    fflush(stdout);
}
