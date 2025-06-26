
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/token.h"

token_t *init_token(char *name, ttype_t ttype, type_t type, size_t line)
{
    token_t *token = (token_t *)calloc(1, sizeof(token_t));
    if (!token)
    {
        perror("Could Not Allocate Token ");
        return NULL;
    }

    token->name = name;
    token->ttype = ttype;
    token->type = type;
    token->line = line;

    return token;
}

int tokcmp(token_t *t1, token_t *t2)
{
    if (t1->ttype - t2->ttype)
        return t1->ttype - t2->ttype;

    if (t1->type - t2->type)
        return t1->type - t2->type;

    return strcmp(t1->name, t2->name);
}
