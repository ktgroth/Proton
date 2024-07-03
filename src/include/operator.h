
#ifndef OPERATOR_H_
#define OPERATOR_H_

typedef struct {
    int precedence;
    int arguments;
    char *name;
} oper, *op_t;

op_t init_operator(int, int, char*);
void free_operator(op_t);

#endif
