
#ifndef PROTON_AST
#define PROTON_AST

#include <stdint.h>

#include "token.h"

typedef struct ast_t ast_t;
typedef struct ast_children_t ast_children_t;

struct ast_t {
    char *name;
    ttype_t atype;
    type_t type;
    union {
        size_t line;
        size_t power;
        size_t addr;
        int64_t value_i;
        double value_f;
    };
    ast_children_t *children;
    ast_t *context;
};

struct ast_children_t {
    ast_t **child;
    size_t size;
};

ast_t *init_ast(char *name, ttype_t atype, type_t type, void *info, ast_t *context);
void add_child(ast_t *ast, ast_t *child);

#endif
