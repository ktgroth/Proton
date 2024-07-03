
#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"
#include "ast.h"
#include "operator.h"
#include "deque.h"
#include "hashtable.h"
#include "hashmap.h"

deque_t shunt(lexer_t);
deque_t form_ast(deque_t);
deque_t solve(deque_t);

#endif
