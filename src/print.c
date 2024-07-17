
#include <stdio.h>

#include "include/print.h"

static int tabs = 0;

void print_token(token_t tok, FILE *buffer) {
    for (int i = 0; i < tabs; ++i)
        fprintf(buffer, "- ");

    switch (tok->type) {
        case ID:
            fprintf(buffer, "[ID]       %s\n", tok->name);
            break;

        case NUM:
            fprintf(buffer, "[NUM]      %s\n", tok->name);
            break;

        case STRING:
            fprintf(buffer, "[STRING]   %s\n", tok->name);
            break;

        case COLON:
            fprintf(buffer, "[COLON]    :\n");
            break;
            
        case ARROW:
            fprintf(buffer, "[ARROW]    ->\n");
            break;
            
        case COMMA:
            fprintf(buffer, "[COMMA]    ,\n");
            break;

        case SEMI:
            fprintf(buffer, "[SEMI]     ;\n");
            break;
            
        case EQUALS:
            fprintf(buffer, "[EQUALS]   =\n");
            break;

        case AND:
            fprintf(buffer, "[AND]      &\n");
            break;
            
        case OR:
            fprintf(buffer, "[OR]       |\n");
            break;
            
        case XOR:
            fprintf(buffer, "[XOR]      ^\n");
            break;
            
        case MUL:
            fprintf(buffer, "[MUL]      *\n");
            break;
            
        case DIV:
            fprintf(buffer, "[DIV]      /\n");
            break;
            
        case ADD:
            fprintf(buffer, "[ADD]      +\n");
            break;
            
        case SUB:
            fprintf(buffer, "[SUB]      -\n");
            break;
            
        case LPAREN:
            fprintf(buffer, "[LPAREN]   (\n");
            break;
            
        case RPAREN:
            fprintf(buffer, "[RPAREN]   )\n");
            break;
            
        case LBRACE:
            fprintf(buffer, "[LBRACE]   {\n");
            break;
            
        case RBRACE:
            fprintf(buffer, "[RBRACE]   }\n");
            break;

        
        default:
            fprintf(stderr, "\033[0;31m");
            fprintf(stderr, "[UNKNOWN TOKEN] <%d>\n", tok->type);
            fprintf(stderr, "\033[0m");
    }
}

void print_ast(ast_t ast, FILE *buffer) {
    for (int i = 0; i < tabs; ++i)
        fprintf(buffer, "- ");

    switch (ast->type) {
        case TYPE:
            fprintf(buffer, "[TYPE]     %s\n", ast->name);
            break;

        case EXP:
            fprintf(buffer, "[EXP]        \n");
            ++tabs;
            for (int i = 1; i < tabs; ++i)
                fprintf(buffer, "- ");
            fprintf(buffer, "CHILDREN:\n");
            print_deque(ast->children, buffer);
            --tabs;
            fprintf(buffer, "\n");
            break;

        case ASSIGN:
            fprintf(buffer, "[ASSIGN]   %s\n", ast->name);
            ++tabs;
            for (int i = 1; i < tabs; ++i)
                fprintf(buffer, "- ");
            fprintf(buffer, "CHILDREN:\n");
            print_deque(ast->children, buffer);
            --tabs;
            fprintf(buffer, "\n");
            break;

        case READ:
            fprintf(buffer, "[READ]     %s\n", ast->name);
            break;

        case ARG:
            fprintf(buffer, "[ARG]      %s\n", ast->name);
            break;

        case DEF:
            fprintf(buffer, "[DEF]      %s\n", ast->name);
            ++tabs;
            for (int i = 1; i < tabs; ++i)
                fprintf(buffer, "- ");
            fprintf(buffer, "CHILDREN:\n");
            print_deque(ast->children, buffer);

            for (int i = 1; i < tabs; ++i)
                fprintf(buffer, "- ");
            fprintf(buffer, "ARGS:\n");
            print_deque(ast->children, buffer);
            --tabs;
            fprintf(buffer, "\n");
            break;

        case CALL:
            fprintf(buffer, "[CALL]     %s\n", ast->name);
            ++tabs;
            for (int i = 1; i < tabs; ++i)
                fprintf(buffer, "-");
            fprintf(buffer, "ARGS:\n");
            print_deque(ast->args, buffer);
            --tabs;
            fprintf(buffer, "\n");
            break;

        
        default:
            fprintf(stderr, "\033[0;31m");
            fprintf(stderr, "[UNKNOWN AST] <%d>\n", ast->type);
            fprintf(stderr, "\033[0m");
    }
}

void print_deque(deque_t dq, FILE *buffer) {
    deque_node_t curr = dq->head;

    while (curr) {
        switch (curr->type) {
            case TOKEN:
                print_token(curr->data, buffer);
                break;
            
            case AST:
                print_ast(curr->data, buffer);
                break;

            case OP:
                print_operator(curr->data, buffer);
                break;
        
            case PAIR:
                print_pair(curr->data, buffer);
                break;

            case NONE:
                fprintf(buffer, "%s\n", (char*)curr->data);
                break;
        }
        curr = curr->next;
    }
}

void print_operator(op_t op, FILE *buffer) {
    for (int i = 0; i < tabs; ++i)
        fprintf(buffer, "- ");

    fprintf(buffer, "[OPERATOR] %s %d %d\n", op->name, op->precedence, op->arguments);
}

void print_pair(pair_t pair, FILE *buffer) {
    for (int i = 0; i < tabs; ++i)
        fprintf(buffer, "- ");

    fprintf(buffer, "[PAIR]     %s\n", pair->name);
    ++tabs;
    switch (pair->type) {
        case TOKEN:
            print_token(pair->data, buffer);
            break;

        case AST:
            print_ast(pair->data, buffer);
            break;
        
        case OP:
            print_operator(pair->data, buffer);
            break;
        
        case PAIR:
            print_pair(pair->data, buffer);
            break;

        case NONE:
            for (int i = 0; i < tabs; ++i)
                fprintf(buffer, "- ");
            fprintf(buffer, "%s\n", (char*)pair->data);
            break;
    }
    --tabs;
}
