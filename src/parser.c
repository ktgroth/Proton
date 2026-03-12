
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/parser.h"


static const char *token_name(TokenKind k)
{
    switch (k) {
        case TOK_EOF: return "EOF";
        case TOK_INT: return "INT";
        case TOK_IF: return "IF";
        case TOK_RETURN: return "RETURN";
        case TOK_ELSE: return "ELSE";
        case TOK_IDENT: return "IDENT";
        case TOK_NUMBER: return "NUMBER";
        case TOK_LPAREN: return "(";
        case TOK_RPAREN: return ")";
        case TOK_LBRACE: return "{";
        case TOK_RBRACE: return "}";
        case TOK_SEMI: return ";";
        case TOK_ASSIGN: return "=";
        case TOK_EQ: return "==";
        case TOK_NE: return "!=";
        case TOK_LT: return "<";
        case TOK_LE: return "<=";
        case TOK_GT: return ">";
        case TOK_GE: return ">=";
        case TOK_PLUS: return "+";
        case TOK_MINUS: return "-";
        case TOK_STAR: return "*";
        case TOK_DIV: return "/";
    }
    return "?";
}

typedef struct LocalVar
{
    char *name;
    int offset;
    struct LocalVar *next;
} LocalVar;


static inline void next(Parser *p)
{
    p->current = lexer_next_token(p->lexer);
}

static void expect(Parser *p, TokenKind kind)
{
    if (p->current.kind != kind)
    {
        fprintf(stderr, "[PARSER ERROR] Unexpected Token\n");
        exit(1);
    }

    next(p);
}

static AST *new_node(ASTKind kind, AST *lhs, AST *rhs, int value)
{
    AST *n = (AST *)calloc(1, sizeof(AST));
    if (!n)
    {
        perror("Could not allocate new AST node");
        exit(1);
    }

    n->kind = kind;
    n->lhs = lhs;
    n->rhs = rhs;
    n->value = value;
    return n;
}

static AST *parse_expr(Parser *p);
static AST *parse_compare(Parser *p);
static AST *parse_add(Parser *p);
static int is_compare_op(TokenKind kind);
static AST *parse_stmt(Parser *p);
static AST *parse_block(Parser *p);
static AST *parse_function(Parser *p);

static LocalVar *find_local(Parser *p, const char *name)
{
    for (LocalVar *v = p->locals; v; v = v->next)
    {
        if (strcmp(v->name, name) == 0)
            return v;
    }
    return NULL;
}

static LocalVar *add_local(Parser *p, const char *name)
{
    LocalVar *v = (LocalVar *)calloc(1, sizeof(LocalVar));
    if (!v)
    {
        perror("Could not allocate local variable");
        exit(1);
    }

    v->name = strdup(name);
    v->offset = (++p->local_count) * 8;
    v->next = p->locals;
    p->locals = v;
    return v;
}

static AST *parse_factor(Parser *p)
{
    if (p->current.kind == TOK_NUMBER)
    {
        AST *n = new_node(AST_NUM, NULL, NULL, p->current.value);
        next(p);
        return n;
    }

    if (p->current.kind == TOK_IDENT)
    {
        char *name = p->current.lexeme;
        next(p);

        if (p->current.kind == TOK_LPAREN)
        {
            next(p);
            expect(p, TOK_RPAREN);
            AST *call = new_node(AST_CALL, NULL, NULL, 0);
            call->name = name;
            return call;
        }

        LocalVar *v = find_local(p, name);
        if (!v)
        {
            fprintf(stderr, "Undefined variable: %s\n", name);
            exit(1);
        }

        AST *n = new_node(AST_VAR, NULL, NULL, 0);
        n->name = name;
        n->offset = v->offset;
        return n;
    }

    if (p->current.kind == TOK_LPAREN)
    {
        next(p);
        AST *e = parse_expr(p);
        expect(p, TOK_RPAREN);
        return e;
    }

    fprintf(stderr, "Expected Factor\n");
    exit(1);
}

static AST *parse_term(Parser *p)
{
    AST *node = parse_factor(p);
    
    while (p->current.kind == TOK_STAR ||
           p->current.kind == TOK_DIV)
    {
        TokenKind op = p->current.kind;
        next(p);

        AST *rhs = parse_factor(p);

        if (op == TOK_STAR)
            node = new_node(AST_MUL, node, rhs, 0);
        else
            node = new_node(AST_DIV, node, rhs, 0);
    }

    return node;
}

static AST *parse_expr(Parser *p)
{
    return parse_compare(p);
}

static int is_compare_op(TokenKind kind)
{
    return kind == TOK_EQ || kind == TOK_NE ||
           kind == TOK_LT || kind == TOK_LE ||
           kind == TOK_GT || kind == TOK_GE;
}

static AST *parse_compare(Parser *p)
{
    AST *node = parse_add(p);
    while (is_compare_op(p->current.kind))
    {
        TokenKind op = p->current.kind;
        next(p);
        AST *rhs = parse_add(p);

        switch (op)
        {
            case TOK_EQ: node = new_node(AST_EQ, node, rhs, 0); break;
            case TOK_NE: node = new_node(AST_NE, node, rhs, 0); break;
            case TOK_LT: node = new_node(AST_LT, node, rhs, 0); break;
            case TOK_LE: node = new_node(AST_LE, node, rhs, 0); break;
            case TOK_GT: node = new_node(AST_GT, node, rhs, 0); break;
            case TOK_GE: node = new_node(AST_GE, node, rhs, 0); break;
            default: break;
        }
    }

    return node;
}

static AST *parse_add(Parser *p)
{
    AST *node = parse_term(p);
    while (p->current.kind == TOK_PLUS ||
           p->current.kind == TOK_MINUS)
    {
        TokenKind op = p->current.kind;
        next(p);

        AST *rhs = parse_term(p);

        if (op == TOK_PLUS)
            node = new_node(AST_ADD, node, rhs, 0);
        else
            node = new_node(AST_SUB, node, rhs, 0);
    }

    return node;
}

static AST *parse_stmt(Parser *p)
{
    if (p->current.kind == TOK_RETURN)
    {
        expect(p, TOK_RETURN);
        AST *e = parse_expr(p);
        expect(p, TOK_SEMI);
        return new_node(AST_RETURN, e, NULL, 0);
    }

    if (p->current.kind == TOK_LBRACE)
        return parse_block(p);

    if (p->current.kind == TOK_IF)
    {
        expect(p, TOK_IF);
        expect(p, TOK_LPAREN);
        AST *cond = parse_expr(p);
        expect(p, TOK_RPAREN);
        AST *then_branch = parse_stmt(p);

        AST *else_branch = NULL;
        if (p->current.kind == TOK_ELSE)
        {
            expect(p, TOK_ELSE);
            else_branch = parse_stmt(p);
        }

        AST *node = new_node(AST_IF, cond, then_branch, 0);
        node->else_branch = else_branch;
        return node;
    }

    if (p->current.kind == TOK_INT)
    {
        expect(p, TOK_INT);
        if (p->current.kind != TOK_IDENT)
        {
            fprintf(stderr, "Expected identifier after int\n");
            exit(1);
        }

        char *name = p->current.lexeme;
        if (find_local(p, name))
        {
            fprintf(stderr, "Redeclared variable: %s\n", name);
            exit(1);
        }

        LocalVar *v = add_local(p, name);
        AST *var = new_node(AST_VAR, NULL, NULL, 0);
        var->name = name;
        var->offset = v->offset;
        next(p);

        AST *init = NULL;
        if (p->current.kind == TOK_ASSIGN)
        {
            expect(p, TOK_ASSIGN);
            init = parse_expr(p);
        }

        expect(p, TOK_SEMI);
        AST *decl = new_node(AST_VAR_DECL, var, init, 0);
        return decl;
    }

    if (p->current.kind == TOK_IDENT)
    {
        char *name = p->current.lexeme;
        LocalVar *v = find_local(p, name);
        if (!v)
        {
            fprintf(stderr, "Undefined variable: %s\n", name);
            exit(1);
        }

        AST *var = new_node(AST_VAR, NULL, NULL, 0);
        var->name = name;
        var->offset = v->offset;
        next(p);

        expect(p, TOK_ASSIGN);
        AST *rhs = parse_expr(p);
        expect(p, TOK_SEMI);
        return new_node(AST_ASSIGN, var, rhs, 0);
    }
    
    fprintf(stderr, "Unknown statement\n");
    exit(1);
}

static AST *parse_block(Parser *p)
{
    expect(p, TOK_LBRACE);
    AST *first = NULL;
    AST *last = NULL;

    while (p->current.kind != TOK_RBRACE && p->current.kind != TOK_EOF)
    {
        AST *stmt = parse_stmt(p);
        if (!first)
            first = stmt;
        else
            last->next = stmt;
        last = stmt;
    }

    expect(p, TOK_RBRACE);
    AST *block = new_node(AST_BLOCK, first, NULL, 0);
    return block;
}

static AST *parse_function(Parser *p)
{
    p->locals = NULL;
    p->local_count = 0;
    expect(p, TOK_INT);
    if (p->current.kind != TOK_IDENT)
    {
        fprintf(stderr, "[PARSER ERROR] Expected function name\n");
        exit(1);
    }
    char *name = p->current.lexeme;
    expect(p, TOK_IDENT);
    expect(p, TOK_LPAREN);
    expect(p, TOK_RPAREN);
    AST *body = parse_block(p);
    AST *func = new_node(AST_FUNCTION, body, NULL, 0);
    func->name = name;
    func->stack_size = p->local_count * 8;
    return func;
}

void init_parser(Parser *p, Lexer *l)
{
    p->lexer = l;
    p->locals = NULL;
    p->local_count = 0;
    next(p);
}

AST *parse_program(Parser *p)
{
    AST *first = NULL;
    AST *last = NULL;

    while (p->current.kind != TOK_EOF)
    {
        AST *func = parse_function(p);
        if (!first)
            first = func;
        else
            last->next = func;
        last = func;
    }

    AST *program = new_node(AST_PROGRAM, first, NULL, 0);
    return program;
}
