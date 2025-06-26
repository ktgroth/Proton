
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/hashmap.h"

pair_t *init_pair(char *value, size_t line)
{
    pair_t *pair = (pair_t *)calloc(1, sizeof(pair_t));
    if (!pair)
    {
        perror("Could Not Allocate Pair ");
        return NULL;
    }

    pair->value = value;
    pair->line = line;
    pair->next = NULL;

    return pair;
}

hashmap_t *init_hashmap(size_t size)
{
    hashmap_t *hmap = (hashmap_t *)calloc(1, sizeof(hashmap_t));
    if (!hmap)
    {
        perror("Could Not Allocate Hashmap ");
        return NULL;
    }

    hmap->size = size;
    hmap->values = (pair_t **)calloc(size, sizeof(pair_t *));

    return hmap;
}

size_t hash(size_t size, char *value)
{
    size_t h = 0;
    for (size_t i = 0; i < strlen(value); ++i)
        h += value[i];

    return h % size;
}

void hashmap_add(hashmap_t *hmap, char *value, size_t line)
{
    size_t h = hash(hmap->size, value);
    pair_t *curr = hmap->values[h], *temp = hmap->values[h];
    while (curr)
    {
        temp = curr;
        curr = curr->next;
    }

    if (temp)
        temp->next = init_pair(value, line);
    else
        hmap->values[h] = init_pair(value, line);
}

void hashmap_remove(hashmap_t *hmap, char *value)
{
    size_t h = hash(hmap->size, value);
    pair_t *curr = hmap->values[h], *temp = hmap->values[h];
    while (curr && strcmp(curr->value, value))
    {
        temp = curr;
        curr = curr->next;
    }

    if (curr)
    {
        temp->next = curr->next;
        free(curr->value);
        free(curr);
    }
}

int search_hashmap(hashmap_t *hmap, char *value)
{
    size_t h = hash(hmap->size, value);
    pair_t *curr = hmap->values[h];
    while (curr && strcmp(curr->value, value))
        curr = curr->next;
        
    if (curr)
        return curr->line;
    return 0;
}
