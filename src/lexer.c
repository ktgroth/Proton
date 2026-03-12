
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "include/lexer.h"


static void advance(Lexer *l)
{
    l->pos++;
    l->current = l->src[l->pos];
}

static void skip_whitespace(Lexer *l)
{
    while (isspace(l->current))
        advance(l);
}

static Token make_token(TokenKind kind)
{
    Token t = { kind, NULL, 0 };
    return t;
}

static Token number(Lexer *l)
{
    int value = 0;
    
    while (isdigit(l->current))
    {
        value = value * 10 + (l->current - '0');
        advance(l);
    }

    Token t = { TOK_NUMBER, NULL, value };
    return t;
}

static Token identifier_or_keyword(Lexer *l)
{
    int start = l->pos;

    while (isalnum(l->current) || l->current == '_')
        advance(l);

    int len = l->pos - start;
    char *name = strndup(l->src + start, len);

    if (strcmp(name, "int") == 0)
    {
        free(name);
        return make_token(TOK_INT);
    }

    if (strcmp(name, "return") == 0)
    {
        free(name);
        return make_token(TOK_RETURN);
    }

    if (strcmp(name, "if") == 0)
    {
        free(name);
        return make_token(TOK_IF);
    }

    if (strcmp(name, "else") == 0)
    {
        free(name);
        return make_token(TOK_ELSE);
    }

    Token t = { TOK_IDENT, name, 0 };
    return t;
}

void init_lexer(Lexer *l, const char *src)
{
    l->src = src;
    l->pos = 0;
    l->current = src[0];
}

Token lexer_next_token(Lexer *l)
{
    skip_whitespace(l);

    if (l->current == '\0')
        return make_token(TOK_EOF);

    if (isdigit(l->current))
        return number(l);
    if (isalpha(l->current) || l->current == '_')
        return identifier_or_keyword(l);
    
    char c = l->current;
    advance(l);

    switch (c) {
        case '(': return make_token(TOK_LPAREN);
        case ')': return make_token(TOK_RPAREN);
        case '{': return make_token(TOK_LBRACE);
        case '}': return make_token(TOK_RBRACE);
        case ';': return make_token(TOK_SEMI);
        case '=':
                if (l->current == '=')
                {
                    advance(l);
                    return make_token(TOK_EQ);
                }
                return make_token(TOK_ASSIGN);
        case '!':
            if (l->current == '=')
            {
                advance(l);
                return make_token(TOK_NE);
            }
            fprintf(stderr, "Unexpected character: '!'\n");
            exit(1);
        case '<':
            if (l->current == '=')
            {
                advance(l);
                return make_token(TOK_LE);
            }
            return make_token(TOK_LT);
        case '>':
            if (l->current == '=')
            {
                advance(l);
                return make_token(TOK_GE);
            }
            return make_token(TOK_GT);
        case '+': return make_token(TOK_PLUS);
        case '-': return make_token(TOK_MINUS);
        case '*': return make_token(TOK_STAR);
        case '/': return make_token(TOK_DIV);
    }

    fprintf(stderr, "Unexpected character: '%c'\n", c);
    exit(1);
}

