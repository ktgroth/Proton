
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/token.h"

token_t init_token(int type, char *name) {
    token_t t = (token_t)calloc(1, sizeof(token));
    if (!t) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[ERROR] Could not allocate memory for token <'%d', '%s'>!\n", type, name);
        exit(1);
    }

    t->type = type;
    t->name = name;

    return t;
}

void free_token(token_t tok) {
    if (tok) {
        switch (tok->type) {
            case ID:
            case NUM:
            case STRING:
                free(tok->name);
        }
        tok->type = 0;
        free(tok);
    }
}

int tokcmp(token_t tok1, token_t tok2) {
    if (tok1->type != tok2->type)
        return tok1->type - tok2->type;

    return strcmp(tok1->name, tok2->name);
}
