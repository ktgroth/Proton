
#ifndef PROTON_LEXER_
#define PROTON_LEXER_

#include <stddef.h>

#include "token.h"

typedef struct
{
    size_t       i;
    char         c;
    const char  *program;
    size_t       size;
    size_t       line;
    size_t       col;
} lexer_t;


lexer_t *init_lexer(const char *program);
void     free_lexer(lexer_t *lexer);

void     advance(lexer_t *lexer);
token_t *advance_with(lexer_t *lexer, token_t *token);

token_t *next_id(lexer_t *lexer);
token_t *next_num(lexer_t *lexer);
token_t *next_string(lexer_t *lexer);
token_t *next_token(lexer_t *lexer);

#endif

