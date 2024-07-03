
#ifndef TOKEN_H_
#define TOKEN_H_

enum {
    ID = 0,
    NUM,
    STRING,

    COLON,
    ARROW,
    COMMA,
    SEMI,
    
    EQUALS,
    AND,
    OR,
    XOR,
    MUL,
    DIV,
    ADD,
    SUB,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    TOKEN_EOF,
};


typedef struct {
    int type;
    char *name;
} token, *token_t;


token_t init_token(int, char*);
void free_token(token_t);

int tokcmp(token_t, token_t);

#endif
