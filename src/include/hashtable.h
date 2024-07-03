
#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "deque.h"

typedef struct {
    deque_t *values;
    int size;
} hashtable, *hashtable_t;

hashtable_t init_hashtable(int);
void free_hashtable(hashtable_t);

void hashtable_add(hashtable_t, char*);
void hashtable_remove(hashtable_t, char*);
int hashtable_contains(hashtable_t, char*);

#endif
