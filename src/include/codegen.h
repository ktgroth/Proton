#ifndef CODEGEN_H_
#define CODEGEN_H_

#include <stdio.h>

#include "ast.h"


void gen_program(AST *ast, FILE *out);

#endif

