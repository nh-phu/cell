#ifndef __LEXER_H__
#define __LEXER_H__

enum token_type {
    TOK_WORD,
    TOK_PIPE,    // |
    TOK_SEMI,    // ;
    TOK_IN,      // <
    TOK_OUT,     // >
    TOK_OUT_APP, // >>
    TOK_END,
    TOK_ERROR,
};

struct token {
    enum token_type type;
    char *text;
};

struct token *lex(char *input);
void free_tokens(struct token *tokens);

#endif
