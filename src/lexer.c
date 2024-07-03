
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "include/lexer.h"

lexer_t init_lexer(char *program) {
    lexer_t lex = (lexer_t)calloc(1, sizeof(lexer));
    if (!lex) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[ERROR] Could not allocate memory for lexer!\n");
        exit(1);
    }

    lex->program = strdup(program);
    lex->size = strlen(program);
    lex->ch = program[lex->pos = 0];

    return lex;
}

void free_lexer(lexer_t lex) {
    if (lex) {
        lex->ch = 0;
        lex->pos = 0;
        lex->size = 0;
        free(lex->program);
        free(lex);
    }
}

void advance(lexer_t lex) {
    if (lex->pos > lex->size) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[LEXER] Advancing past EOF!\n");
        exit(1);
    }

    lex->ch = lex->program[++lex->pos];
}

void skip_whitespace(lexer_t lex) {
    while (isspace(lex->ch))
        advance(lex);
}

void skip_comment(lexer_t lex) {
    while (lex->ch != '\n')
        advance(lex);
}

token_t advance_with(lexer_t lex, token_t tok) {
    advance(lex);
    return tok;
}

token_t next_id(lexer_t lex) {
    char *id = (char*)calloc(1, sizeof(char));

    while (isalnum(lex->ch) || lex->ch == '_') {
        char temp[2] = { lex->ch, '\0' };
        id = realloc(id, (strlen(id) + 2) * sizeof(char));
        id = strcat(id, temp);
        advance(lex);
    }

    return init_token(ID, id);
}

token_t next_num(lexer_t lex) {
    char *num = (char*)calloc(1, sizeof(char));

    while (isdigit(lex->ch) || lex->ch == '.') {
        char temp[2] = { lex->ch, '\0' };
        num = realloc(num, (strlen(num) + 2) * sizeof(char));
        num = strcat(num, temp);
        advance(lex);
    }

    return init_token(NUM, num);
}

void read_char(lexer_t lex, char *chars) {
    if (lex->ch == '\\') {
        advance(lex);

        switch (lex->ch) {
        case 'n':
            chars[0] = '\n';
            break;
        
        case 'r':
            chars[0] = '\r';
            break;

        case 't':
            chars[0] = '\t';
            break;

        default:
            break;
        }
    } else {
        chars[0] = lex->ch;
    }
}

token_t next_string(lexer_t lex) {
    char *string = (char*)calloc(1, sizeof(char));

    advance(lex);
    while (lex->ch != '"') {
        char temp[2] = { '\0', '\0' };
        read_char(lex, temp);
        string = realloc(string, (strlen(string) + 2) * sizeof(char));
        string = strcat(string, temp);
        advance(lex);
    }
    advance(lex);

    return init_token(STRING, string);
}

token_t next_token(lexer_t lex) {
    skip_whitespace(lex);
    while (lex->ch == '#') {
        skip_comment(lex);
        skip_whitespace(lex);
    }

    if (lex->pos >= lex->size)
        return init_token(TOKEN_EOF, NULL);

    if (isalpha(lex->ch))
        return next_id(lex);

    if (isdigit(lex->ch) || lex->ch == '.')
        return next_num(lex);

    switch (lex->ch) {
        case '"':
            return next_string(lex);

        case ':':
            return advance_with(lex, init_token(COLON, NULL));

        case '=':
            return advance_with(lex, init_token(EQUALS, "="));

        case '-':
            advance(lex);
            if (lex->ch == '>')
                return advance_with(lex, init_token(ARROW, NULL));
            else
                return init_token(SUB, "-");

        case ',':
            return advance_with(lex, init_token(COMMA, NULL));

        case ';':
            return advance_with(lex, init_token(SEMI, NULL));
            
        case '&':
            return advance_with(lex, init_token(AND, "&"));

        case '|':
            return advance_with(lex, init_token(OR, "|"));

        case '^':
            return advance_with(lex, init_token(XOR, "^"));

        case '*':
            return advance_with(lex, init_token(MUL, "*"));

        case '/':
            return advance_with(lex, init_token(DIV, "/"));

        case '+':
            return advance_with(lex, init_token(ADD, "+"));

        case '(':
            return advance_with(lex, init_token(LPAREN, "("));

        case ')':
            return advance_with(lex, init_token(RPAREN, ")"));

        case '{':
            return advance_with(lex, init_token(LBRACE, NULL));

        case '}':
            return advance_with(lex, init_token(RBRACE, NULL));

        default:
            fprintf(stderr, "\033[0;31m");
            fprintf(stderr, "[LEXER] Unknown Token <%c>!\n", lex->ch);
            exit(1);
    }

    return NULL;
}
