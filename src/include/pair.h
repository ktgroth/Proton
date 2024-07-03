
#ifndef PAIR_H_
#define PAIR_H_

#include "token.h"
#include "ast.h"
#include "operator.h"

typedef struct {
    char *name;
    void *data;
    int type;
} pair, *pair_t;

pair_t init_pair();
void free_pair(pair_t);

#endif
