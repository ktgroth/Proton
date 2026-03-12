#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"
#include "ast.h"


typedef struct
{
    Lexer *lexer;
    Token current;
    struct LocalVar *locals;
    int local_count;
} Parser;

void init_parser(Parser *p, Lexer *l);
AST *parse_program(Parser *p);

#endif
