
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/print.h"
#include "include/hashmap.h"
#include "include/deque.h"
#include "include/token.h"
#include "include/ast.h"
#include "include/operator.h"

hashmap_t init_hashmap(int size) {
    hashmap_t hmap = (hashmap_t)calloc(1, sizeof(hashmap));
    if (!hmap) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[ERROR] Could not allocate memory for hashmap!\n");
        exit(1);
    }

    hmap->size = size;
    hmap->values = (deque_t*)calloc(size, sizeof(deque_t));
    if (!hmap->values) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[ERROR] Could not allocate memory for hashmap's table!\n");
        exit(1);
    }

    for (int i = 0; i < size; ++i)
        hmap->values[i] = init_deque();

    return hmap;
}

static int hash(char *str) {
    int value = 0;
    for (int i = 0; i < strlen(str); ++i)
        value += (int)str[i];

    return value;
}

void hashmap_add(hashmap_t hmap, char *str, void *data, int type) {
    int index = hash(str) % hmap->size;

    pair_t par = init_pair(str, data, type);
    deque_push_back(hmap->values[index], par, PAIR);
}


void hashmap_remove(hashmap_t hmap, char *str) {
    int index = hash(str) % hmap->size;
    
    deque_node_t curr = hmap->values[index]->head;
    while (strcmp(curr->data, str))
        curr = curr->next;

    if (curr) {
        if (curr->prev)
            curr->prev->next = curr->next;
        else
            hmap->values[index]->head = curr->next;
        
        if (curr->next)
            curr->next->prev = curr->prev;
        else
            hmap->values[index]->tail = curr->prev;
        
        switch (curr->type) {
            case TOKEN:
                free_token(((pair_t)curr->data)->data);
                break;

            case AST:
                free_ast(((pair_t)curr->data)->data);
                break;

            case OP:
                free_operator(((pair_t)curr->data)->data);
                break;

            case PAIR:
                free_pair(((pair_t)curr->data)->data);
                break;

            case NONE:
                free(((pair_t)curr->data)->data);
                break;
        }

        curr->prev = NULL;
        curr->next = NULL;
        free(curr);
    }
}


int hashmap_contains(hashmap_t hmap, char *str) {
    int index = hash(str) % hmap->size;

    deque_node_t curr = hmap->values[index]->head;
    while (curr) {
        pair_t item = (pair_t)curr->data;
        if (!strcmp(str, item->name))
            return 1;
        curr = curr->next;
    }

    return 0;
}

void *hashmap_get(hashmap_t hmap, char *str) {
    int index = hash(str) % hmap->size;

    deque_node_t curr = hmap->values[index]->head;
    while (curr) {
        pair_t item = (pair_t)curr->data;
        if (!strcmp(str, item->name))
            return item->data;
        curr = curr->next;
    }

    return NULL;
}
