#ifndef AST_H_
#define AST_H_

//
// program      -> function*
// function     -> "int" ident "(" ")" "{" stmt* "}"
// stmt         -> "return" expr ";"
//                 | "if" "(" expr ")" stmt ("else" stmt)?
//                 | "{" stmt* "}"
//                 | "int" ident ("=" expr)? ";"
//                 | ident "=" expr ";"
// expr         -> compare
// compare      -> term (("+" | "-") term)* (("==" | "!=" | "<" | "<=" | ">" | ">=") term (("+" | "-") term)*)*
// term         -> factor (("*" | "/") factor)*
// factor       -> number | ident | ident "(" ")" | "(" expr ")"
//

typedef enum
{
    AST_PROGRAM,
    AST_FUNCTION,
    AST_BLOCK,
    AST_NUM,
    AST_VAR,
    AST_CALL,
    AST_VAR_DECL,
    AST_ASSIGN,
    AST_ADD,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_EQ,
    AST_NE,
    AST_LT,
    AST_LE,
    AST_GT,
    AST_GE,
    AST_RETURN,
    AST_IF,
} ASTKind;

typedef struct AST
{
    ASTKind kind;
    struct AST *lhs;
    struct AST *rhs;
    struct AST *else_branch;
    struct AST *next;
    char *name;
    int value;
    int offset;
    int stack_size;
} AST;

#endif
