
#ifndef PROTON_LEXER
#define PROTON_LEXER

#include <string.h>

#include "token.h"

typedef struct
{
    char *program;
    size_t size, i;
    char c;
} lexer_t;

lexer_t *init_lexer(char *program);
token_t *next_token(lexer_t *lexer);

#endif
