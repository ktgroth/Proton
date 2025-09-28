
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/token.h"


token_t *init_token(token_type_e type, const char *name, size_t line, size_t col)
{
    token_t *token = (token_t *)calloc(1, sizeof(token_t));
    if (!token)
    {
        perror("Could not allocate new Token");
        return NULL;
    }

    token->type = type;
    token->name = name;
    token->line = line;
    token->col = col;

    return token;
}

void free_token(token_t *token)
{
    if (!token)
        return;

    free(token);
}

#define TPRINTC(type, token) \
    case type:  \
        printf("%-15s %s, %zu, %zu\n", #type, (token)->name, (token)->line, (token)->col); \
        break;


void print_token(const token_t *token)
{
    switch (token->type)
    {
        TPRINTC(TOKEN_ID, token);
        TPRINTC(TOKEN_INT, token);
        TPRINTC(TOKEN_FLOAT, token);
        TPRINTC(TOKEN_STRING, token);

        TPRINTC(TOKEN_LPAREN, token);
        TPRINTC(TOKEN_RPAREN, token);
        TPRINTC(TOKEN_LBRACE, token);
        TPRINTC(TOKEN_RBRACE, token);
        TPRINTC(TOKEN_LBRACK, token);
        TPRINTC(TOKEN_RBRACK, token);

        TPRINTC(TOKEN_AND, token);
        TPRINTC(TOKEN_BAND, token);
        TPRINTC(TOKEN_OR, token);
        TPRINTC(TOKEN_BOR, token);
        TPRINTC(TOKEN_XOR, token);
        TPRINTC(TOKEN_NOT, token);
        TPRINTC(TOKEN_FLIP, token);

        TPRINTC(TOKEN_ADD, token);
        TPRINTC(TOKEN_SUB, token);
        TPRINTC(TOKEN_NEG, token);
        TPRINTC(TOKEN_MUL, token);
        TPRINTC(TOKEN_DIV, token);
        TPRINTC(TOKEN_MOD, token);
        TPRINTC(TOKEN_EQUAL, token);

        TPRINTC(TOKEN_LTE, token);
        TPRINTC(TOKEN_LT, token);
        TPRINTC(TOKEN_NEQ, token);
        TPRINTC(TOKEN_EQ, token);
        TPRINTC(TOKEN_GTE, token);
        TPRINTC(TOKEN_GT, token);

        TPRINTC(TOKEN_SEMI, token);

        TPRINTC(TOKEN_EOF, token);

        default:
            fprintf(stderr, "[LEXICAL ERROR] Unknown Token!");
    }
}

