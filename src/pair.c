
#include <stdio.h>
#include <stdlib.h>

#include "include/pair.h"

pair_t init_pair(char *name, void *data, int type) {
    pair_t par = (pair_t)calloc(1, sizeof(pair));
    if (!par) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[ERROR] Could not allocate memory for pair!\n");
        exit(1);
    }

    par->name = name;
    par->data = data;
    par->type = type;

    return par;
}

void free_pair(pair_t par) {
    if (par) {
        free(par->name);
        switch (par->type) {
            case TOKEN:
                free_token(par->data);
                break;

            case AST:
                free_ast(par->data);
                break;

            case OP:
                free_operator(par->data);
                break;

            case PAIR:
                free_pair(par->data);
                break;

            case NONE:
                free(par->data);
                break;
        }
        par->type = 0;
        free(par);
    }
}