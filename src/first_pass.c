
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include "include/first_pass.h"
#include "include/globals.h"

fp_parser_t *init_fp_parser(char *program)
{
    fp_parser_t *parser = (fp_parser_t *)calloc(1, sizeof(fp_parser_t));
    if (!parser)
    {
        perror("Could Not Allocate Parser ");
        return NULL;
    }

    parser->lexer = init_lexer(program);
    if (!parser->lexer)
        return NULL;

    parser->token = next_token(parser->lexer);
    if (!parser->token)
        return NULL;

    size_t line = 0;
    parser->context[0] = init_ast("PROGRAM", SYMBOL, TYPE_INT, &line, NULL);
    if (!parser->context[0])
        return NULL;

    return parser;
}

int parser_eat(fp_parser_t *parser, ttype_t type) {
    token_t *token = parser->token;
    if (token->ttype != type)
    {
        errno = ENOSYS;
        fprintf(stderr, "{%s %d %d} ", token->name, token->ttype, type);
        perror("Cannot Eat Token ");
        return 1;
    }
    
    free(parser->token);
    parser->token = next_token(parser->lexer);
    return (long)parser->token;
}

ast_t *parse_expr(fp_parser_t *parser)
{
    token_t *token = parser->token;
    size_t line = token->line;
    parser->context[2] = init_ast("EXPR", EXPR, TYPE_INT, &line, parser->context[1]);

    while (parser->token->ttype != SEMI)
        (void)parse(parser);
    parser_eat(parser, SEMI);

    ast_t *expr = parser->context[2];
    parser->context[2] = NULL;
    return expr;
}

ast_t *parse_var(fp_parser_t *parser)
{
    token_t *token = parser->token;

    if (!parser->context[2])
        return parse_expr(parser);
 
    ast_t *var = init_ast(token->name, token->ttype, token->type, 0, parser->context[2]);
    parser_eat(parser, VAR);

    return var;
}

ast_t *parse_keyword(fp_parser_t *parser) {
    return NULL;
}

ast_t *parse_op(fp_parser_t *parser) {
    token_t *token = parser->token;

    size_t power = search_hashmap(ops, token->name);
    if (power == -(1UL)) 
    {
        errno = ENOSYS;
        fprintf(stderr, "{%s} ", token->name);
        perror("Unknown Operator ");
        return NULL;
    }

    ast_t *op = init_ast(token->name, OP, token->type, &power, parser->context[2]);
    parser_eat(parser, parser->token->ttype);

    return op;
}

ast_t *parse_id(fp_parser_t *parser)
{
    token_t *token = parser->token;

    if (token->ttype == VAR) 
        parse_var(parser);
    else if (token->ttype == KEYWORD)
        parse_keyword(parser);

    size_t line = 0;
    ast_t *id = init_ast(token->name, token->ttype, TYPE_INT, &line, parser->context[2]);
    parser_eat(parser, ID);

    return id;
}

ast_t *parse_symbol(fp_parser_t *parser)
{
    token_t *token = parser->token;
    ast_t *symbol = init_ast(token->name, token->ttype, token->type, &token->line, parser->context[0]);
    parser_eat(parser, SYMBOL);

    parser->context[1] = symbol;
    return symbol;
}

int64_t parse_bin(char *bin)
{
    int64_t value = 0;
    size_t power = 1;
    size_t s = strlen(bin);
    for (size_t i = 1; i <= s; ++i)
    {
        if (bin[s-i] == '1')
            value += power;
        power <<= 1;
    }

    return value;
}

ast_t *parse_num(fp_parser_t *parser)
{
    token_t *token = parser->token;

    ast_t *num;
    int64_t value_i;
    double value_f;
    switch (token->type)
    {
        case TYPE_POINTER:
        case TYPE_HEX:
            value_i = strtoll(token->name, NULL, 16);
            num = init_ast(token->name, token->ttype, token->type, &value_i, parser->context[2]);
            break;
        
        case TYPE_BINARY:
            value_i = parse_bin(token->name);
            printf("%ld\n", value_i);
            num = init_ast(token->name, token->ttype, token->type, &value_i, parser->context[2]);
            break;
        
        case TYPE_OCTAL:
            value_i = strtoll(token->name, NULL, 8);
            num = init_ast(token->name, token->ttype, token->type, &value_i, parser->context[2]);
            break;

        case TYPE_INT:
            value_i = strtoll(token->name, NULL, 10);
            num = init_ast(token->name, token->ttype, token->type, &value_i, parser->context[2]);
            break;

        case TYPE_FLOAT:
            value_f = atof(token->name);
            num = init_ast(token->name, token->ttype, token->type, &value_f, parser->context[2]);
            break;

        default:
            errno = ENOSYS;
            fprintf(stderr, "{%d} ", token->type);
            perror("Unexpected Type ");
            parser_eat(parser, NUM);
            return NULL;
    }
    parser_eat(parser, NUM);

    return num;
}

ast_t *parse_string(fp_parser_t *parser)
{
    token_t *token = parser->token;
    ast_t *str = init_ast(token->name, token->ttype, token->type, &token->line, parser->context[2]);
    parser_eat(parser, STRING);
    return str;
}

ast_t *parse_read(fp_parser_t *parser)
{
    return NULL;
}

ast_t *parse(fp_parser_t *parser)
{
    token_t *token = parser->token;
    switch (token->ttype)
    {
        case ID:
            return parse_id(parser);
            break;

        case SYMBOL:
            return parse_symbol(parser);
            break;
        
        case NUM:
            return parse_num(parser);
            break;
        
        case STRING:
            return parse_string(parser);
            break;
        
        case VAR:
            return parse_var(parser);
            break;

        case LBRACK:
            return parse_read(parser);
            break;

        case ASSIGN:
        case EQUALS:
        case BIT_AND:
        case AND:
        case BIT_OR:
        case OR:
        case XOR:
        case MUL:
        case DIV:
        case ADD:
        case SUB:
            return parse_op(parser);
            break;

        default:
            errno = ENOSYS;
            fprintf(stderr, "{%s %d %d} ", token->name, token->ttype, token->type);
            perror("Unexpected Token ");    
    }

    return parser->context[0];
}

ast_t *parse_init(fp_parser_t *parser)
{
    token_t *token;
    while ((token = parser->token)->ttype != PEOF)
    {
        (void)parse(parser);
    }

    return parser->context[0];
}
