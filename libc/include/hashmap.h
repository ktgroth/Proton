
#ifndef LIBC_HASHMAP
#define LIBC_HASHMAP

#include <stdlib.h>

typedef struct pair_t {
    char *value;
    size_t line;
    struct pair_t *next;
} pair_t;

typedef struct {
    pair_t **values;
    size_t size;
} hashmap_t;

pair_t *init_pair(char *value, size_t line);

hashmap_t *init_hashmap(size_t size);
size_t hash(size_t size, char *value);

void hashmap_add(hashmap_t *hmap, char *value, size_t line);
void hashmap_remove(hashmap_t *hmap, char *value);

int search_hashmap(hashmap_t *hmap, char *value);

#endif
