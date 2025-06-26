
#ifndef PROTON_PARSER
#define PROTON_PARSER

#include "lexer.h"
#include "ast.h"

typedef struct
{
    lexer_t *lexer;
    token_t *token;
    ast_t *context[3];
} fp_parser_t;

fp_parser_t *init_fp_parser(char *program);

int parser_eat(fp_parser_t *parser, ttype_t type);
ast_t *parse_id(fp_parser_t *parser);
ast_t *parse_symbol(fp_parser_t *parser);
ast_t *parse_num(fp_parser_t *parser);
ast_t *parse_string(fp_parser_t *parser);
ast_t *parse_read(fp_parser_t *parser);
ast_t *parse(fp_parser_t *parser);
ast_t *parse_init(fp_parser_t *parser);

#endif
