
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "include/lexer.h"


lexer_t *init_lexer(const char *program)
{
    lexer_t *lexer = (lexer_t *)calloc(1, sizeof(lexer_t));
    if (!lexer)
    {
        perror("Could not allocate Lexer");
        return NULL;
    }

    lexer->i = 0;
    lexer->program = program;
    lexer->size = strlen(program);
    lexer->c = lexer->program[lexer->i];
    lexer->line = 1;
    lexer->col = 1;

    return lexer;
}

void free_lexer(lexer_t *lexer)
{
    if (!lexer)
        return;

    free(lexer);
}

void advance(lexer_t *lexer)
{
    if (lexer->i >= lexer->size)
        return;

    ++lexer->col;
    lexer->c = lexer->program[++lexer->i];
}

token_t *advance_with(lexer_t *lexer, token_t *token)
{
    advance(lexer);
    return token;
}

void skip_whitespace(lexer_t *lexer)
{
    while (isspace(lexer->c))
    {
        if (lexer->c == '\n')
        {
            lexer->col = 1;
            ++lexer->line;
        }

        advance(lexer);
    }
}

void skip_comment(lexer_t *lexer)
{
    while (lexer->c != '\n')
        advance(lexer);
}

token_t *next_id(lexer_t *lexer)
{
    size_t line = lexer->line, col = lexer->col;

    char *id = calloc(1, sizeof(char));
    while (isalnum(lexer->c) || lexer->c == '_')
    {
        size_t len = strlen(id);
        id = realloc(id, (len + 2) * sizeof(char));
        id[len+0] = lexer->c;
        id[len+1] = '\0';

        advance(lexer);
    }

    return init_token(TOKEN_ID, id, lexer->line, lexer->col);
}

token_t *next_num(lexer_t *lexer)
{
    size_t line = lexer->line, col = lexer->col;

    char *num = calloc(1, sizeof(char));
    while (isdigit(lexer->c) || lexer->c == '.')
    {
        size_t len = strlen(num);
        num = realloc(num, (len + 2) * sizeof(char));
        num[len+0] = lexer->c;
        num[len+1] = '\0';

        advance(lexer);
    }

    return init_token(TOKEN_INT, num, line, col);
}

token_t *next_string(lexer_t *lexer)
{
    size_t line = lexer->line, col = lexer->col;

    advance(lexer);
    char *str = calloc(1, sizeof(char));
    while (lexer->c != '"')
    {
        size_t len = strlen(str);
        str = realloc(str, (len + 2) * sizeof(char));
        str[len+0] = lexer->c;
        str[len+1] = '\0';

        advance(lexer);
    }

    advance(lexer);
    return init_token(TOKEN_STRING, str, line, col);
}

#define NEW_TOKEN(type, lexer, name) \
    return init_token(type, name, lexer->line, lexer->col);

#define ADVANCE_WITH_NEW_TOKEN(type, lexer, name) \
    return advance_with(lexer, init_token(type, name, lexer->line, lexer->col));

token_t *next_token(lexer_t *lexer)
{
    skip_whitespace(lexer);
    if (lexer->c == '\\')
        skip_comment(lexer);

    if (lexer->i >= lexer->size)
        return init_token(TOKEN_EOF, "EOF", lexer->line, lexer->col);

    if (isalpha(lexer->c) || lexer->c == '_')
        return next_id(lexer);

    if (isdigit(lexer->c))
        return next_num(lexer);

    if (lexer->c == '"')
        return next_string(lexer);

    switch (lexer->c)
    {
        case '(': ADVANCE_WITH_NEW_TOKEN(TOKEN_LPAREN, lexer, "(");
        case ')': ADVANCE_WITH_NEW_TOKEN(TOKEN_RPAREN, lexer, ")");
        case '{': ADVANCE_WITH_NEW_TOKEN(TOKEN_LBRACE, lexer, "{");
        case '}': ADVANCE_WITH_NEW_TOKEN(TOKEN_RBRACE, lexer, "}");
        case '[': ADVANCE_WITH_NEW_TOKEN(TOKEN_LBRACK, lexer, "[");
        case ']': ADVANCE_WITH_NEW_TOKEN(TOKEN_RBRACK, lexer, "]");

        case '&': {
            advance(lexer);
            if (lexer->c == '&')
                ADVANCE_WITH_NEW_TOKEN(TOKEN_AND, lexer, "&&");

            NEW_TOKEN(TOKEN_BAND, lexer, "&");
        }

        case '|': {
            advance(lexer);
            if (lexer->c == '|')
                ADVANCE_WITH_NEW_TOKEN(TOKEN_OR, lexer, "||");

            NEW_TOKEN(TOKEN_BOR, lexer, "|");
        }

        case '^': ADVANCE_WITH_NEW_TOKEN(TOKEN_XOR, lexer, "^");
        case '!': {
            advance(lexer);
            if (lexer->c == '=')
                ADVANCE_WITH_NEW_TOKEN(TOKEN_NEQ, lexer, "!=");

            NEW_TOKEN(TOKEN_NOT, lexer, "!");
        }

        case '~': ADVANCE_WITH_NEW_TOKEN(TOKEN_FLIP, lexer, "~");

        case '+': ADVANCE_WITH_NEW_TOKEN(TOKEN_ADD, lexer, "+");
        case '-': ADVANCE_WITH_NEW_TOKEN(TOKEN_SUB, lexer, "-");
        case '*': ADVANCE_WITH_NEW_TOKEN(TOKEN_MUL, lexer, "*");
        case '/': ADVANCE_WITH_NEW_TOKEN(TOKEN_DIV, lexer, "/");
        case '%': ADVANCE_WITH_NEW_TOKEN(TOKEN_MOD, lexer, "%");
        case '=': {
            advance(lexer);
            if (lexer->c == '=')
                ADVANCE_WITH_NEW_TOKEN(TOKEN_EQ, lexer, "==");

            NEW_TOKEN(TOKEN_EQUAL, lexer, "=");
        }

        case '<': {
            advance(lexer);
            if (lexer->c == '=')
                ADVANCE_WITH_NEW_TOKEN(TOKEN_LTE, lexer, "<=");

            NEW_TOKEN(TOKEN_LT, lexer, "<");
        }

        case '>': {
            advance(lexer);
            if (lexer->c == '=')
                ADVANCE_WITH_NEW_TOKEN(TOKEN_GTE, lexer, ">=");

            NEW_TOKEN(TOKEN_GT, lexer, ">");
        }

        case ';':
            ADVANCE_WITH_NEW_TOKEN(TOKEN_SEMI, lexer, ";");

        default:
            fprintf(stderr, "Unhandled character '%c'", lexer->c);
            return NULL;
    }
}

