
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "include/ast.h"

ast_t *init_ast(char *name, ttype_t atype, type_t type, void *info, ast_t *context)
{
    ast_t *ast = (ast_t *)calloc(1, sizeof(ast_t));
    if (!ast)
    {
        perror("Could Not Allocate AST ");
        return NULL;
    }

    ast->name = name;
    ast->atype = atype;
    ast->type = type;
    
    switch (type)
    {
        case TYPE_STRING:
            ast->line = *((size_t *)info);
            break;

        case TYPE_POINTER:
            ast->addr = *((size_t *)info);
            break;

        case TYPE_BINARY:
        case TYPE_INT:
        case TYPE_HEX:
        case TYPE_OCTAL:
            ast->value_i = *((int64_t *)info);
            break;

        case TYPE_FLOAT:
            ast->value_f = *((double *)info);
            break;

        case TYPE_OP:
            ast->power = *((size_t *)info);
            break;

        default:
            ast->line = 0;
    }
    ast->children = (ast_children_t *)calloc(1, sizeof(ast_children_t));
    if (!ast->children) {
        perror("Could Not Allocate AST Children ");
        free(ast);
        return NULL;
    }

    ast->children->child = (ast_t **)calloc(0, sizeof(ast_t *));
    ast->children->size = 0;

    ast->context = context;
    if (context)
        add_child(context, ast);

    return ast;
}

void add_child(ast_t *ast, ast_t *child)
{
    ast_children_t *children = ast->children;
    size_t s = children->size;
    children->child = realloc(children->child, ++children->size * sizeof(ast_t *));
    children->child[s] = child;
}
