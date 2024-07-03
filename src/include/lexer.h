
#ifndef LEXER_H_
#define LEXER_H_

#include <stdlib.h>

#include "token.h"

typedef struct {
    char *program;
    size_t size;
    char ch;
    int pos;
} lexer, *lexer_t;

lexer_t init_lexer(char*);
void free_lexer(lexer_t);

token_t next_token(lexer_t);

#endif
