
#ifndef PROTON_TOKEN_
#define PROTON_TOKEN_

#include "stddef.h"

typedef enum
{
    TOKEN_ID,
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING,

    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LBRACK,
    TOKEN_RBRACK,

    TOKEN_AND,
    TOKEN_BAND,
    TOKEN_OR,
    TOKEN_BOR,
    TOKEN_XOR,
    TOKEN_NOT,
    TOKEN_FLIP,

    TOKEN_ADD,
    TOKEN_SUB,
    TOKEN_NEG,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_EQUAL,

    TOKEN_LTE,
    TOKEN_LT,
    TOKEN_NEQ,
    TOKEN_EQ,
    TOKEN_GTE,
    TOKEN_GT,

    TOKEN_SEMI,

    TOKEN_EOF,
} token_type_e;

typedef struct
{
    token_type_e     type;
    const char      *name;
    size_t           line;
    size_t           col;
} token_t;


token_t *init_token(token_type_e type, const char *name, size_t line, size_t col);
void     free_token(token_t *token);
void     print_token(const token_t *token);

#endif

