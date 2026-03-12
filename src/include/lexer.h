#ifndef LEXER_H_
#define LEXER_H_

typedef enum 
{
    TOK_INT,
    TOK_RETURN,
    TOK_IF, TOK_ELSE,
    TOK_IDENT,
    TOK_NUMBER,
    TOK_LPAREN, TOK_RPAREN,
    TOK_LBRACE, TOK_RBRACE,
    TOK_SEMI,
    TOK_ASSIGN,
    TOK_EQ, TOK_NE,
    TOK_LT, TOK_LE,
    TOK_GT, TOK_GE,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_DIV,
    TOK_EOF,
} TokenKind;

typedef struct
{
    TokenKind kind;
    char *lexeme;
    int value;
} Token;

typedef struct
{
    const char *src;
    int pos;
    char current;
} Lexer;


void init_lexer(Lexer *l, const char *src);
Token lexer_next_token(Lexer *l);

#endif

