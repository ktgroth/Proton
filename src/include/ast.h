
#ifndef AST_H_
#define AST_H_

#include "deque.h"

enum {
    TYPE,
    EXP,

    ASSIGN,
    READ,
    ARG,
    DEF,
    CALL,
};

typedef struct {
    int type;
    char *name;
    deque_t children;
    deque_t args;
} sast, *ast_t;

ast_t init_ast(int, char*);
void free_ast(ast_t);


#endif
