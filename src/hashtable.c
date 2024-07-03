
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/hashtable.h"

hashtable_t init_hashtable(int size) {
    hashtable_t htab = (hashtable_t)calloc(1, sizeof(hashtable));
    if (!htab) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[ERROR] Could not allocate memory for hashtable!\n");
        exit(1);
    }

    htab->size = size;
    htab->values = (deque_t*)calloc(size, sizeof(deque_t));
    if (!htab->values) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[ERROR] Could not allocate memory for hashtable's table!\n");
        exit(1);
    }

    for (int i = 0; i < size; ++i)
        htab->values[i] = init_deque();

    return htab;
}

void free_hashtable(hashtable_t htab) {
    if (htab) {
        for (int i = 0; i < htab->size; ++i)
            free_deque(htab->values[i]);

        htab->size = 0;
        free(htab->values);
        free(htab);
    }
}

static int hash(char *str) {
    int value = 0;
    for (int i = 0; i < strlen(str); ++i)
        value += (int)str[i];

    return value;
}

void hashtable_add(hashtable_t htab, char *str) {
    int index = hash(str) % htab->size;
    deque_push_back(htab->values[index], str, NONE);
}


void hashtable_remove(hashtable_t htab, char *str) {
    int index = hash(str) % htab->size;
    
    deque_node_t curr = htab->values[index]->head;
    while (strcmp(curr->data, str))
        curr = curr->next;

    if (curr) {
        if (curr->prev)
            curr->prev->next = curr->next;
        else
            htab->values[index]->head = curr->next;
        
        if (curr->next)
            curr->next->prev = curr->prev;
        else
            htab->values[index]->tail = curr->prev;
        
        free(curr->data);
        curr->prev = NULL;
        curr->next = NULL;
        free(curr);
    }
}


int hashtable_contains(hashtable_t htab, char *str) {
    int index = hash(str) % htab->size;
    
    deque_node_t curr = htab->values[index]->head;
    while (curr) {
        char *name = curr->data;
        if (!strcmp(str, name))
            return 1;
        curr = curr->next;
    }

    return 0;
}
