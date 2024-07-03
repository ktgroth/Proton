
#include <stdio.h>
#include <stdlib.h>

#include "include/operator.h"

op_t init_operator(int precedence, int arguments, char *name) {
    op_t op = (op_t)calloc(1, sizeof(oper));
    if (!op) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[ERROR] Could not allocate memory for operator!\n");
        exit(1);
    }

    op->precedence = precedence;
    op->arguments = arguments;
    op->name = name;

    return op;
}

void free_operator(op_t op) {
    if (op) {
        op->precedence = 0;
        op->arguments = 0;
        free(op->name);
        free(op);
    }
}
