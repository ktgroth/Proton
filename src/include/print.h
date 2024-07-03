
#ifndef PRINT_H_
#define PRINT_H_

#include "token.h"
#include "ast.h"
#include "deque.h"
#include "operator.h"
#include "pair.h"

void print_token(token_t);
void print_ast(ast_t);
void print_deque(deque_t);
void print_operator(op_t);
void print_pair(pair_t);

#endif
