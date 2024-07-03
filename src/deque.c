
#include <stdio.h>

#include "include/ast.h"
#include "include/deque.h"

deque_node_t init_deque_node(void *data, deque_node_t next, deque_node_t prev, int type) {
    deque_node_t node = (deque_node_t)calloc(1, sizeof(deque_node));
    if (!node) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[ERROR] Could not allocate memory for deque node\n");
        exit(1);
    }

    node->type = type;
    node->data = data;
    node->next = next;
    node->prev = prev;

    return node;
}

void free_deque_node(deque_node_t node) {
    if (node) {
        node->type = 0;
        node->next = NULL;
        node->prev = NULL;
        free(node);
    }
}

deque_t init_deque(void) {
    deque_t dq = (deque_t)calloc(1, sizeof(deque));
    if (!dq) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[ERROR] Could not allocate memory for deque\n");
        exit(1);
    }

    dq->size = 0;
    dq->head = NULL;
    dq->tail = NULL;

    return dq;
}

void free_deque(deque_t dq) {
    if (dq) {
        while (dq->size)
            deque_pop_front(dq);

        free(dq);
    }
}

void deque_push_front(deque_t dq, void *data, int type) {
    dq->head = init_deque_node(data, dq->head, NULL, type);
    
    if (dq->size)
        dq->head->next->prev = dq->head;
    else
        dq->tail = dq->head;
    
    ++dq->size;
}

void deque_push_back(deque_t dq, void *data, int type) {
    dq->tail = init_deque_node(data, NULL, dq->tail, type);
    
    if (dq->size)
        dq->tail->prev->next = dq->tail;
    else
        dq->head = dq->tail;
    
    ++dq->size;
}

void *deque_get(deque_t dq, size_t offset) {
    if (offset >= dq->size) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[DEQUE] <GET> Out of bounds! {SIZE : %zu} \n", dq->size);
        fprintf(stderr, "\033[37;6m");
        return NULL;
    }

    deque_node_t curr = dq->head;
    while (offset--)
        curr = curr->next;

    return curr->data;
}

void *deque_front(deque_t dq) {
    if (dq->head)
        return dq->head->data;

    return NULL;
}

void *deque_back(deque_t dq) {
    if (dq->tail)
        return dq->tail->data;

    return NULL;
}

void *deque_pop_front(deque_t dq) {
    if (!dq->size) {
        printf("Popping from the front of an empty deque!\n");
        return NULL;
    }

    deque_node_t curr = dq->head;
    dq->head = dq->head->next;
    --dq->size;
    
    if (!dq->size)
        dq->tail = dq->head;

    void *data = curr->data;
    free_deque_node(curr);

    return data;
}

void *deque_pop_back(deque_t dq) {
    if (!dq->size) {
        printf("Popping from the back of an empty deque!\n");
        return NULL;
    }

    deque_node_t curr = dq->tail;
    dq->tail = dq->tail->prev;
    --dq->size;

    if (!dq->size)
        dq->head = dq->tail;

    void *data = curr->data;
    free_deque_node(curr);

    return data;
}
