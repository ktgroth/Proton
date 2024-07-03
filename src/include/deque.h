
#ifndef DEQUE_H_
#define DEQUE_H_

#include <stdlib.h>

enum {
    NONE = 0,
    TOKEN,
    AST,
    OP,
    PAIR,
};

typedef struct node {
    void *data;
    int type;
    struct node *next;
    struct node *prev;
} deque_node, *deque_node_t;

typedef struct {
    size_t size;
    deque_node_t head;
    deque_node_t tail;
} deque, *deque_t;

deque_t init_deque(void);
void free_deque(deque_t);

void deque_push_front(deque_t, void*, int);
void deque_push_back(deque_t, void*, int);
void* deque_get(deque_t, size_t);
void* deque_front(deque_t);
void* deque_back(deque_t);
void* deque_pop_front(deque_t);
void* deque_pop_back(deque_t);

#endif
