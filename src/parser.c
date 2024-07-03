
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/parser.h"
#include "include/print.h"

static deque_t tokens = NULL;
static deque_t output = NULL;
static deque_t holding = NULL;

static hashtable_t keywords = NULL;
static hashtable_t vars = NULL;
static hashtable_t funcs = NULL;

static hashmap_t ops = NULL;

int isoperator(char *str) {
    if (!str)
        return 0;

    return hashmap_contains(ops, str);
}

ast_t parse_id(token_t);
ast_t parse_exp(int);

ast_t parse(token_t tok) {
    print_token(tok);
    
    switch (tok->type) {
        case ID:
            return parse_id(tok);
            break;

        case NUM:
        case STRING:
            return (ast_t)tok;
            break;

        default:
            fprintf(stderr, "\033[0;31m");
            fprintf(stderr, "[PARSER] Unexpected token <%d>\n", tok->type);
            exit(1);
    }
}

void parse_comma() {
    print_token(deque_front(tokens));
    if (((token_t)deque_front(tokens))->type != COMMA) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[PARSER] Expected a ','");
        exit(1);
    }

    token_t tok = deque_pop_front(tokens);
    free_token(tok);
}

void parse_semi() {
    print_token(deque_front(tokens));
    if (((token_t)deque_front(tokens))->type != SEMI) {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[PARSER] Expected a ';'");
        exit(1);
    }

    token_t tok = deque_pop_front(tokens);
    free_token(tok);
}

ast_t parse_arg(token_t tok) {

}

ast_t parse_id(token_t tok) {
    ast_t ast;
    char *name = tok->name;
    
    token_t next = deque_front(tokens);
    if (hashtable_contains(keywords, name)) {
        if (!strcmp(name, "print")) {
            ast = init_ast(CALL, strdup(name));
            free_token(tok);

            token_t next = deque_pop_front(tokens);
            while (next->type != RPAREN) {
                deque_push_back(ast->args, parse_exp(COMMA), AST);
                next = deque_pop_front(tokens);
            }
            print_token(next);
            free_token(next);

            parse_semi();
        } else if (!strcmp(name, "return")) {

        }
    } else if (hashtable_contains(vars, name)) {
        ast = init_ast(READ, strdup(name));
        free_token(tok);
    } else if (hashtable_contains(funcs, name)) {
        ast = init_ast(CALL, strdup(name));
        free_token(tok);

        
    } else if (next->type == COLON) {
        ast = init_ast(ASSIGN, strdup(name));
        hashtable_add(vars, ast->name);
        free_token(tok);

        next = deque_pop_front(tokens);
        free_token(next);

        next = deque_pop_front(tokens);
        ast_t type = init_ast(TYPE, strdup(next->name));
        deque_push_back(ast->children, type, AST);
        free_token(next);

        next = deque_front(tokens);
        if (next->type != EQUALS) {
            deque_push_front(tokens, init_token(NUM, "0"), TOKEN);
            deque_push_front(tokens, init_token(EQUALS, "="), TOKEN);
        }

        ast_t exp = parse_exp(SEMI);
        print_ast(exp);
        deque_push_back(ast->children, exp, AST);
    } else if (next->type == LPAREN) {
        ast = init_ast(DEF, strdup(name));
        free_token(tok);
        free_token(next);

        
    } else {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[PARSER] Unexpected token ");
        print_token(next);
        exit(1);
    }

    return ast;
}

ast_t parse_exp(int end_type) {
    ast_t exp = init_ast(EXP, NULL);

    token_t next;
    while ((next = deque_pop_front(tokens))->type != end_type) {
        print_token(next);

        if (next->type == LPAREN) {
            deque_push_front(holding, next, TOKEN);
        } else if (next->type == RPAREN) {
            token_t front = deque_front(holding);
            while (holding->size && front->type != LPAREN) {
                deque_push_back(exp->children, front, TOKEN);
                deque_pop_front(holding);
                front = deque_front(holding);
            }

            if (!holding->size) {
                printf("RPARENS SPECIAL CASE\n");
                deque_push_front(tokens, next, TOKEN);
                return exp;
            } else
                deque_pop_front(holding);
        } else if (isoperator(next->name)) {
            op_t new_op = (op_t)hashmap_get(ops, next->name);

            token_t front = deque_front(holding);
            while (holding->size && front->type != LPAREN)
                if (isoperator(front->name)) {
                    op_t front_op = (op_t)hashmap_get(ops, front->name);
                    if (front_op->precedence >= new_op->precedence) {
                        deque_push_back(exp->children, front, TOKEN);
                        deque_pop_front(holding);
                        front = deque_front(holding);
                    } else
                        break;
                }
            
            deque_push_front(holding, next, TOKEN);
        } else if (next->type == ID)
            deque_push_back(exp->children, parse(next), AST);
        else
            deque_push_back(exp->children, parse(next), TOKEN);
    }
    free_token(next);

    while (holding->size)
        deque_push_back(exp->children, deque_pop_front(holding), TOKEN);

    return exp;
}


void init_keywords() {
    hashtable_add(keywords, "print");
    hashtable_add(keywords, "return");
    hashtable_add(keywords, "const");
}

void init_operators() {
    hashmap_add(ops, "=", init_operator(-1, 1, "="), OP);
    hashmap_add(ops, "*", init_operator(4, 2, "*"), OP);
    hashmap_add(ops, "/", init_operator(4, 2, "/"), OP);
    hashmap_add(ops, "+", init_operator(2, 2, "+"), OP);
    hashmap_add(ops, "-", init_operator(2, 2, "-"), OP);
    hashmap_add(ops, "(", init_operator(0, 2, "("), OP);
    hashmap_add(ops, ")", init_operator(0, 2, ")"), OP);
}

deque_t shunt(lexer_t lex) {
    tokens = init_deque();
    token_t tok;
    while ((tok = next_token(lex))->type != TOKEN_EOF)
        deque_push_back(tokens, tok, TOKEN);
    free_lexer(lex);

    printf("Tokens:\n");
    print_deque(tokens);
    printf("\n\n");

    output = init_deque();
    holding = init_deque();
    keywords = init_hashtable(20);
    ops = init_hashmap(20);

    init_keywords();
    init_operators();

    vars = init_hashtable(20);
    funcs = init_hashtable(20);

    hashtable_add(funcs, "print");

    while (tokens->size) {
        tok = deque_pop_front(tokens);
        deque_push_back(output, parse(tok), AST);
    }

    printf("\n\nHolding:\n");
    print_deque(holding);

    free_deque(tokens);
    free_deque(holding);
    return output;
}


deque_t solve(deque_t toks) {
    tokens = toks;
    output = init_deque();


    free_deque(tokens);
    return output;
}
