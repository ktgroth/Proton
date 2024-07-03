
#ifndef HASHMAP_H_
#define HASHMAP_H_

#include "deque.h"
#include "pair.h"

typedef struct {
    deque_t *values;
    int size;
} hashmap, *hashmap_t;

hashmap_t init_hashmap(int);
void free_hashmap(hashmap_t);

void hashmap_add(hashmap_t, char*, void*, int);
void hashmap_remove(hashmap_t, char*);
int hashmap_contains(hashmap_t, char*);
void *hashmap_get(hashmap_t, char*);

#endif
