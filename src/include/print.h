
#ifndef PRINT_H_
#define PRINT_H_

#include "token.h"
#include "ast.h"
#include "deque.h"
#include "operator.h"
#include "pair.h"

void print_token(token_t, FILE*);
void print_ast(ast_t, FILE*);
void print_deque(deque_t, FILE*);
void print_operator(op_t, FILE*);
void print_pair(pair_t, FILE*);

#endif
