
#include <stdio.h>

#include "include/ast.h"

ast_t init_ast(int type, char *name) {
    ast_t ast = (ast_t)calloc(1, sizeof(sast));
    if (!ast) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[ERROR] Could not allocate memory for ast\n");
        exit(1);
    }

    ast->type = type;
    ast->name = name;
    
    switch (type) {
        case EXP:
        case ASSIGN:
            ast->children = init_deque();
            break;

        case CALL:
            ast->args = init_deque();
            break;

        case DEF:
            ast->children = init_deque();
            ast->args = init_deque();
            break;

        default:
            break;
    }

    return ast;
}

void free_ast(ast_t ast) {
    if (ast) {
        ast->type = 0;
        free(ast->name);
        if (ast->children)
            free_deque(ast->children);
        free(ast);
    }
}
