
#include <stdio.h>
#include <errno.h>
#include <regex.h>
#include <ctype.h>
#include <stdlib.h>

#include "include/lexer.h"
#include "include/globals.h"

static size_t line = 1;


lexer_t *init_lexer(char *program)
{
    lexer_t *lexer = (lexer_t *)calloc(1, sizeof(lexer_t));
    if (!lexer)
    {
        perror("Could Not Allocate Lexer ");
        return NULL;
    }

    lexer->program = program;
    lexer->size = strlen(program);
    lexer->i = 0;
    lexer->c = program[0];

    return lexer;
}

void advance(lexer_t *lexer)
{
    lexer->i += 1;
    if (lexer->i >= lexer->size)
    {
        lexer->c = '\0';
        return;
    }

    lexer->c = lexer->program[lexer->i];
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
            line += 1;
        advance(lexer);
    }
}

void skip_comment(lexer_t *lexer)
{
    while (lexer->c != '\n')
        advance(lexer);
}

token_t *read_string(lexer_t *lexer)
{
    char *str = (char *)calloc(1, sizeof(char));

    advance(lexer);
    while (lexer->c != '\"')
    {
        size_t s = strlen(str);
        str = realloc(str, (s + 2) * sizeof(char));
        str[s] = lexer->c;
        str[s+1] = '\0';
    
        advance(lexer);
    }
    advance(lexer);
    
    return init_token(str, STRING, TYPE_STRING, line);
}


int regex_hex(char *hex)
{
    regex_t regex;
    int reti;
    char msgbuf[256];

    reti = regcomp(&regex, "^[+-]?0[xX][0-9a-fA-F]+P?$", REG_EXTENDED);
    if (reti)
    {
        fprintf(stderr, "Could not compile regex\n");
        return reti;
    }

    reti = regexec(&regex, hex, 0, NULL, 0);
    regfree(&regex);
    if (!reti)
        return 0;
    else if (reti == REG_NOMATCH)
        return 1;
    else
    {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        return -1;
    }
}

int regex_binary(char *bin)
{
    regex_t regex;
    int reti;
    char msgbuf[256];

    reti = regcomp(&regex, "^0b[0-1]+", REG_EXTENDED);
    if (reti)
    {
        fprintf(stderr, "Could not compile regex\n");
        return reti;
    }

    reti = regexec(&regex, bin, 0, NULL, 0);
    if (!reti)
        return 0;
    else if (reti == REG_NOMATCH)
        return 1;
    else
    {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        return -1;
    }
}

int regex_octal(char *oct)
{
    regex_t regex;
    int reti;
    char msgbuf[256];

    reti = regcomp(&regex, "^0[0-7]+$", REG_EXTENDED);
    if (reti)
    {
        fprintf(stderr, "Could not compile regex\n");
        return reti;
    }

    reti = regexec(&regex, oct, 0, NULL, 0);
    regfree(&regex);
    if (!reti)
        return 0;
    else if (reti == REG_NOMATCH)
        return 1;
    else
    {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        return -1;
    }
}

int regex_int(char *dec)
{
    regex_t regex;
    int reti;
    char msgbuf[256];

    reti = regcomp(&regex, "^[+-]?[0-9][0-9]*$", REG_EXTENDED);
    if (reti)
    {
        fprintf(stderr, "Could not compile regex\n");
        return reti;
    }
    
    reti = regexec(&regex, dec, 0, NULL, 0);
    regfree(&regex);
    if (!reti)
        return 0;
    else if (reti == REG_NOMATCH)
        return 1;
    else
    {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        return -1;
    }
}

int regex_float(char *flt)
{
    regex_t regex;
    int reti;
    char msgbuf[256];

    reti = regcomp(&regex, "^[+-]?([0-9]+)((,|.)([0-9]*))?$", REG_EXTENDED);
    if (reti)
    {
        fprintf(stderr, "Could not compile regex\n");
        return reti;
    }
    
    reti = regexec(&regex, flt, 0, NULL, 0);
    regfree(&regex);
    if (!reti)
        return 0;
    else if (reti == REG_NOMATCH)
        return 1;
    else
    {
        regerror(reti, &regex, msgbuf, sizeof(msgbuf));
        fprintf(stderr, "Regex match failed: %s\n", msgbuf);
        return -1;
    }
}

token_t *read_number(lexer_t *lexer)
{
    char *num = (char *)calloc(1, sizeof(char));

    while (isdigit(lexer->c) || lexer->c == 'x' || lexer->c == 'X' ||
            ((lexer->c >= 'a' && lexer->c <= 'f') || (lexer->c >= 'A' && lexer->c <= 'F')) ||
            lexer->c == '.')
    {
        size_t s = strlen(num);
        num = realloc(num, (s + 2) * sizeof(char));
        num[s] = lexer->c;
        num[s+1] = '\0';

        advance(lexer);
    }

    if (!regex_hex(num))
    {
        if (lexer->c == 'P')
            return advance_with(lexer, init_token(num, NUM, TYPE_POINTER, line));
        return init_token(num, NUM, TYPE_HEX, line);
    }
    
    if (!regex_binary(num))
        return init_token(num, NUM, TYPE_BINARY, line);
    if (!regex_octal(num))
        return init_token(num, NUM, TYPE_OCTAL, line);
    if (!regex_int(num))
        return init_token(num, NUM, TYPE_INT, line);
    if (!regex_float(num))
        return init_token(num, NUM, TYPE_FLOAT, line);

    free(num);    
    errno = ENOSYS;
    perror("Unknown Token ");
    return NULL;
}

token_t *read_id(lexer_t *lexer)
{
    char *id = (char *)calloc(1, sizeof(char));

    while (isalnum(lexer->c) || lexer->c == '_')
    {
        size_t s = strlen(id);
        id = realloc(id, (s + 2) * sizeof(char));
        id[s] = lexer->c;
        id[s+1] = '\0';

        advance(lexer);
    }

    if (search_trie(registers, id))
        return init_token(id, VAR, TYPE_NVAR, line);
    if (search_trie(keywords, id))
        return init_token(id, KEYWORD, TYPE_NVAR, line);
    if (lexer->c == ':')
    {
        if (search_hashmap(symbols, id))
        {
            fprintf(stderr, "Cannot Redefine Symbol");
            return NULL;
        }
        hashmap_add(symbols, id, line);
        return advance_with(lexer, init_token(id, SYMBOL, TYPE_INT, line));
    }

    return init_token(id, ID, TYPE_NVAR, line);
}

token_t *next_token(lexer_t *lexer)
{
    skip_whitespace(lexer);
    if (lexer->c == '#')
        skip_comment(lexer);
    skip_whitespace(lexer);
    if (lexer->i >= lexer->size)
        return init_token("EOF", PEOF, TYPE_NVAR, line);

    if (lexer->c == '\"')
        return read_string(lexer);
    if (isdigit(lexer->c) || lexer->c == '.')
        return read_number(lexer);
    if (isalpha(lexer->c) || lexer->c == '_')
        return read_id(lexer);

    switch (lexer->c)
    {
        case ';':
            return advance_with(lexer, init_token(";", SEMI, TYPE_NVAR, line));
        case ',':
            return advance_with(lexer, init_token(",", COMMA, TYPE_NVAR, line));
        case '=':
            advance(lexer);
            if (lexer->c == '=')
                return advance_with(lexer, init_token("==", EQUALS, TYPE_NVAR, line));
            return init_token("=", ASSIGN, TYPE_NVAR, line);
        case '&':
            advance(lexer);
            if (lexer->c == '&')
                return advance_with(lexer, init_token("&&", AND, TYPE_NVAR, line));
            return init_token("&", BIT_AND, TYPE_NVAR, line);
        case '|':
            advance(lexer);
            if (lexer->c == '|')
                return advance_with(lexer, init_token("||", OR, TYPE_NVAR, line));
            return init_token("|", BIT_OR, TYPE_NVAR, line);
        case '^':
            return advance_with(lexer, init_token("^", XOR, TYPE_NVAR, line));
        case '*':
            return advance_with(lexer, init_token("*", MUL, TYPE_NVAR, line));
        case '/':
            return advance_with(lexer, init_token("/", DIV, TYPE_NVAR, line));
        case '+':
            return advance_with(lexer, init_token("+", ADD, TYPE_NVAR, line));
        case '-':
            advance(lexer);
            if (lexer->c == '>')
                return advance_with(lexer, init_token("->", ARROW, TYPE_NVAR, line));
            return init_token("-", SUB, TYPE_NVAR, line);
        case '(':
            return advance_with(lexer, init_token("(", LPAREN, TYPE_NVAR, line));
        case ')':
            return advance_with(lexer, init_token(")", RPAREN, TYPE_NVAR, line));
        case '{':
            return advance_with(lexer, init_token("{", LBRACE, TYPE_NVAR, line));
        case '}':
            return advance_with(lexer, init_token("}", RBRACE, TYPE_NVAR, line));
        case '[':
            return advance_with(lexer, init_token("[", LBRACK, TYPE_NVAR, line));
        case ']':
            return advance_with(lexer, init_token("]", RBRACK, TYPE_NVAR, line));
        default:
            errno = ENOSYS;
            printf("{%c}", lexer->c);
            perror("Unknown Token ");
            return NULL;
    }
}
