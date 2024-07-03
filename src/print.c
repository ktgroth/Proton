
#include <stdio.h>

#include "include/print.h"

static int tabs = 0;

void print_token(token_t tok) {
    for (int i = 0; i < tabs; ++i)
        printf("- ");

    switch (tok->type) {
        case ID:
            printf("[ID]       %s\n", tok->name);
            break;

        case NUM:
            printf("[NUM]      %s\n", tok->name);
            break;

        case STRING:
            printf("[STRING]   %s\n", tok->name);
            break;

        case COLON:
            printf("[COLON]    :\n");
            break;
            
        case ARROW:
            printf("[ARROW]    ->\n");
            break;
            
        case COMMA:
            printf("[COMMA]    ,\n");
            break;

        case SEMI:
            printf("[SEMI]     ;\n");
            break;
            
        case EQUALS:
            printf("[EQUALS]   =\n");
            break;

        case AND:
            printf("[AND]      &\n");
            break;
            
        case OR:
            printf("[OR]       |\n");
            break;
            
        case XOR:
            printf("[XOR]      ^\n");
            break;
            
        case MUL:
            printf("[MUL]      *\n");
            break;
            
        case DIV:
            printf("[DIV]      /\n");
            break;
            
        case ADD:
            printf("[ADD]      +\n");
            break;
            
        case SUB:
            printf("[SUB]      -\n");
            break;
            
        case LPAREN:
            printf("[LPAREN]   (\n");
            break;
            
        case RPAREN:
            printf("[RPAREN]   )\n");
            break;
            
        case LBRACE:
            printf("[LBRACE]   {\n");
            break;
            
        case RBRACE:
            printf("[RBRACE]   }\n");
            break;

        
        default:
            fprintf(stderr, "\033[0;31m");
            fprintf(stderr, "[UNKNOWN TOKEN] <%d>\n", tok->type);
            fprintf(stderr, "\033[0m");
    }
}

void print_ast(ast_t ast) {
    for (int i = 0; i < tabs; ++i)
        printf("- ");

    switch (ast->type) {
        case TYPE:
            printf("[TYPE]     %s\n", ast->name);
            break;

        case EXP:
            printf("[EXP]        \n");
            ++tabs;
            for (int i = 1; i < tabs; ++i)
                printf("- ");
            printf("CHILDREN:\n");
            print_deque(ast->children);
            --tabs;
            break;

        case ASSIGN:
            printf("[ASSIGN]   %s\n", ast->name);
            ++tabs;
            for (int i = 1; i < tabs; ++i)
                printf("- ");
            printf("CHILDREN:\n");
            print_deque(ast->children);
            --tabs;
            break;

        case READ:
            printf("[READ]     %s\n", ast->name);
            break;

        case ARG:
            printf("[ARG]      %s\n", ast->name);
            break;

        case DEF:
            printf("[DEF]      %s\n", ast->name);
            ++tabs;
            for (int i = 1; i < tabs; ++i)
                printf("- ");
            printf("CHILDREN:\n");
            print_deque(ast->children);

            for (int i = 1; i < tabs; ++i)
                printf("- ");
            printf("ARGS:\n");
            print_deque(ast->children);
            --tabs;
            break;

        case CALL:
            printf("[CALL]     %s\n", ast->name);
            ++tabs;
            for (int i = 1; i < tabs; ++i)
                printf("-");
            printf("ARGS:\n");
            print_deque(ast->args);
            --tabs;
            break;

        
        default:
            fprintf(stderr, "\033[0;31m");
            fprintf(stderr, "[UNKNOWN AST] <%d>\n", ast->type);
            fprintf(stderr, "\033[0m");
    }
}

void print_deque(deque_t dq) {
    deque_node_t curr = dq->head;

    while (curr) {
        switch (curr->type) {
            case TOKEN:
                print_token(curr->data);
                break;
            
            case AST:
                print_ast(curr->data);
                break;

            case OP:
                print_operator(curr->data);
                break;
        
            case PAIR:
                print_pair(curr->data);
                break;

            case NONE:
                printf("%s\n", (char*)curr->data);
                break;
        }
        curr = curr->next;
    }
}

void print_operator(op_t op) {
    for (int i = 0; i < tabs; ++i)
        printf("- ");

    printf("[OPERATOR] %s %d %d\n", op->name, op->precedence, op->arguments);
}

void print_pair(pair_t pair) {
    for (int i = 0; i < tabs; ++i)
        printf("- ");

    printf("[PAIR]     %s\n", pair->name);
    ++tabs;
    switch (pair->type) {
        case TOKEN:
            print_token(pair->data);
            break;

        case AST:
            print_ast(pair->data);
            break;
        
        case OP:
            print_operator(pair->data);
            break;
        
        case PAIR:
            print_pair(pair->data);
            break;

        case NONE:
            for (int i = 0; i < tabs; ++i)
                printf("- ");
            printf("%s\n", (char*)pair->data);
            break;
    }
    --tabs;
}
