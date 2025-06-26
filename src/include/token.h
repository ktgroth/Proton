
#ifndef PROTON_TOKEN
#define PROTON_TOKEN

typedef enum
{
    TYPE_STRING = 0,
    TYPE_POINTER,
    TYPE_BINARY,
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_HEX,
    TYPE_OCTAL,
    TYPE_OP,
    TYPE_NVAR,
} type_t;

typedef enum
{
    ID = 0,
    SYMBOL,
    VAR,
    KEYWORD,
    NUM,
    STRING,

    EXPR,
    ASSIGN,
    EQUALS,
    BIT_AND,
    AND,
    BIT_OR,
    OR,
    XOR,
    MUL,
    DIV,
    ADD,
    SUB,
    OP,

    SEMI,
    ARROW,
    COMMA,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    LBRACK,
    RBRACK,
    PEOF,
} ttype_t;

typedef struct
{
    char *name;
    ttype_t ttype;
    type_t type;
    size_t line;
} token_t;

token_t *init_token(char *name, ttype_t ttype, type_t type, size_t line);
int tokcmp(token_t *t1, token_t *t2);

#endif
