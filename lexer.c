#include "lexer.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int is_op(char c)
{
    return c == '|' || c == ';' || c == '<' || c == '>';
}

static int push_token(struct token **tokens, int *len, int *cap,
                      enum token_type type, char *text)
{
    if (*len + 1 >= *cap) {
        int new_cap = (*cap == 0) ? 32 : (*cap * 2);
        struct token *new_tokens =
            realloc(*tokens, new_cap * sizeof(*new_tokens));
        if (!new_tokens)
            return -1;
        *tokens = new_tokens;
        *cap = new_cap;
    }
    (*tokens)[*len].type = type;
    (*tokens)[*len].text = text;
    (*len)++;
    return 0;
}

struct token *lex(char *input)
{
    struct token *tokens = NULL;
    int len = 0;
    int cap = 0;

    char *r = input;
    char *w = input;

    enum token_type pending = TOK_END;

    while (*r) {
        if (pending != TOK_END) {
            if (push_token(&tokens, &len, &cap, pending, NULL) != 0)
                goto oom;
            pending = TOK_END;
            continue;
        }

        while (*r && isspace((unsigned char)*r))
            r++;
        if (!*r)
            break;

        // Operators
        if (*r == '|') {
            if (push_token(&tokens, &len, &cap, TOK_PIPE, NULL) != 0)
                goto oom;
            r++;
            continue;
        }
        if (*r == ';') {
            if (push_token(&tokens, &len, &cap, TOK_SEMI, NULL) != 0)
                goto oom;
            r++;
            continue;
        }
        if (*r == '<') {
            if (push_token(&tokens, &len, &cap, TOK_IN, NULL) != 0)
                goto oom;
            r++;
            continue;
        }
        if (*r == '>') {
            if (*(r + 1) == '>') {
                if (push_token(&tokens, &len, &cap, TOK_OUT_APP, NULL) != 0)
                    goto oom;
                r += 2;
            } else {
                if (push_token(&tokens, &len, &cap, TOK_OUT, NULL) != 0)
                    goto oom;
                r++;
            }
            continue;
        }

        // WORD
        char *word = w;
        while (*r) {
            if (isspace((unsigned char)*r) || is_op(*r))
                break;

            if (*r == '\\') {
                r++;
                if (!*r) {
                    fprintf(stderr, "syntax error: unfinished escape\\n");
                    (void)push_token(&tokens, &len, &cap, TOK_ERROR, NULL);
                    goto done;
                }
                *w++ = *r++;
                continue;
            }

            if (*r == '\'') {
                r++;
                while (*r && *r != '\'')
                    *w++ = *r++;
                if (*r != '\'') {
                    fprintf(stderr, "syntax error: unterminated quote\\n");
                    (void)push_token(&tokens, &len, &cap, TOK_ERROR, NULL);
                    goto done;
                }
                r++; // closing '
                continue;
            }

            if (*r == '"') {
                r++;
                while (*r && *r != '"') {
                    if (*r == '\\') {
                        r++;
                        if (!*r) {
                            fprintf(stderr,
                                    "syntax error: unfinished escape\\n");
                            (void)push_token(&tokens, &len, &cap, TOK_ERROR,
                                             NULL);
                            goto done;
                        }
                        *w++ = *r++;
                        continue;
                    }
                    *w++ = *r++;
                }
                if (*r != '"') {
                    fprintf(stderr, "syntax error: unterminated quote\\n");
                    (void)push_token(&tokens, &len, &cap, TOK_ERROR, NULL);
                    goto done;
                }
                r++; // closing "
                continue;
            }

            *w++ = *r++;
        }

        // We stopped due to delimiter (whitespace/operator) or end-of-input.
        // If delimiter is an operator, remember it as a pending token and
        // advance r so we can safely NUL-terminate in-place.
        if (*r && is_op(*r)) {
            if (*r == '|') {
                pending = TOK_PIPE;
                r++;
            } else if (*r == ';') {
                pending = TOK_SEMI;
                r++;
            } else if (*r == '<') {
                pending = TOK_IN;
                r++;
            } else if (*r == '>') {
                if (*(r + 1) == '>') {
                    pending = TOK_OUT_APP;
                    r += 2;
                } else {
                    pending = TOK_OUT;
                    r++;
                }
            }
        } else if (*r) {
            // Skip whitespace delimiter(s)
            while (*r && isspace((unsigned char)*r))
                r++;
        }

        *w++ = '\0';
        if (push_token(&tokens, &len, &cap, TOK_WORD, word) != 0)
            goto oom;
    }

done:
    if (push_token(&tokens, &len, &cap, TOK_END, NULL) != 0)
        goto oom;
    return tokens;

oom:
    fprintf(stderr, "out of memory\\n");
    free(tokens);
    return NULL;
}

void free_tokens(struct token *tokens) { free(tokens); }
