
#include <stdlib.h>
#include <string.h>

#include "include/codegen.h"


static int if_label_count = 0;
static int return_label_count = 0;
static int current_return_label = -1;
static void gen_expr(AST *node, FILE *out);
static void gen_stmt(AST *node, FILE *out);


static void gen_expr(AST *node, FILE *out)
{
    switch (node->kind)
    {
        case AST_NUM:
            fprintf(out, "\tmov rax, %d\n", node->value);
            return;

        case AST_ADD:
            gen_expr(node->lhs, out);
            fprintf(out, "\tpush rax\n");
            gen_expr(node->rhs, out);
            fprintf(out, "\tpop rdi\n");
            fprintf(out, "\tadd rax, rdi\n\n");
            return;

        case AST_SUB:
            gen_expr(node->lhs, out);
            fprintf(out, "\tpush rax\n");
            gen_expr(node->rhs, out);
            fprintf(out, "\tmov rdi, rax\n");
            fprintf(out, "\tpop rax\n");
            fprintf(out, "\tsub rax, rdi\n\n");
            return;

        case AST_MUL:
            gen_expr(node->lhs, out);
            fprintf(out, "\tpush rax\n");
            gen_expr(node->rhs, out);
            fprintf(out, "\tpop rdi\n");
            fprintf(out, "\timul rax, rdi\n\n");
            return;

        case AST_DIV:
            gen_expr(node->lhs, out);
            fprintf(out, "\tpush rax\n");
            gen_expr(node->rhs, out);
            fprintf(out, "\tmov rdi, rax\n");
            fprintf(out, "\tpop rax\n");
            fprintf(out, "\tcqo\n");
            fprintf(out, "\tidiv rdi\n\n");
            return;

        case AST_EQ:
        case AST_NE:
        case AST_LT:
        case AST_LE:
        case AST_GT:
        case AST_GE:
            gen_expr(node->lhs, out);
            fprintf(out, "\tpush rax\n");
            gen_expr(node->rhs, out);
            fprintf(out, "\tpop rdi\n");
            fprintf(out, "\tcmp rdi, rax\n");
            switch (node->kind)
            {
                case AST_EQ: fprintf(out, "\tsete al\n"); break;
                case AST_NE: fprintf(out, "\tsetne al\n"); break;
                case AST_LT: fprintf(out, "\tsetl al\n"); break;
                case AST_LE: fprintf(out, "\tsetle al\n"); break;
                case AST_GT: fprintf(out, "\tsetg al\n"); break;
                case AST_GE: fprintf(out, "\tsetge al\n"); break;
                default: break;
            }
            fprintf(out, "\tmovzx rax, al\n\n");
            return;

        case AST_VAR:
            fprintf(out, "\tmov rax, [rbp-%d]\n\n", node->offset);
            return;

        case AST_CALL:
            fprintf(out, "\tcall %s\n\n", node->name);
            return;

        default:
            fprintf(stderr, "Unknown Expression Node\n");
            return;
    }
}

static void gen_stmt(AST *node, FILE *out)
{
    switch (node->kind)
    {
        case AST_RETURN:
            gen_expr(node->lhs, out);
            fprintf(out, "\tjmp .func_end_%d\n", current_return_label);
            return;

        case AST_BLOCK:
            for (AST *stmt = node->lhs; stmt; stmt = stmt->next)
                gen_stmt(stmt, out);
            return;

        case AST_VAR_DECL:
            if (node->rhs)
            {
                gen_expr(node->rhs, out);
                fprintf(out, "\tmov [rbp-%d], rax\n", node->lhs->offset);
            }
            else
            {
                fprintf(out, "\tmov qword [rbp-%d], 0\n", node->lhs->offset);
            }
            return;

        case AST_ASSIGN:
            gen_expr(node->rhs, out);
            fprintf(out, "\tmov [rbp-%d], rax\n", node->lhs->offset);
            return;

        case AST_IF:
            int lbl = if_label_count++;
            int has_else = node->else_branch != NULL;

            gen_expr(node->lhs, out);
            fprintf(out, "\tcmp rax, 0\n");
            
            if (has_else)
                fprintf(out, "\tje .else_%d\n", lbl);
            else
                fprintf(out, "\tje .endif_%d\n", lbl);

            gen_stmt(node->rhs, out);

            if (has_else)
            {
                fprintf(out, "\tjmp .endif_%d\n", lbl);
                fprintf(out, ".else_%d:\n", lbl);
                gen_stmt(node->else_branch, out);
            }

            fprintf(out, ".endif_%d:\n", lbl);
            return;
        
        default:
            fprintf(stderr, "Unknown statement kind\n");
            return;
    }
}

static int has_function(AST *ast, const char *name)
{
    for (AST *func = ast->lhs; func; func = func->next)
    {
        if (func->kind == AST_FUNCTION && func->name && strcmp(func->name, name) == 0)
            return 1;
    }
    return 0;
}

static void gen_function(AST *func, FILE *out)
{
    const char *func_name = (func && func->name) ? func->name : "main";
    int stack_size = func ? func->stack_size : 0;

    if (stack_size % 16 != 0)
        stack_size += 8;

    current_return_label = return_label_count++;

    fprintf(out, "%s:\n", func_name);
    fprintf(out, "\tpush rbp\n");
    fprintf(out, "\tmov rbp, rsp\n\n");
    if (stack_size > 0)
        fprintf(out, "\tsub rsp, %d\n\n", stack_size);

    if (func)
        gen_stmt(func->lhs, out);

    fprintf(out, ".func_end_%d:\n", current_return_label);
    fprintf(out, "\tmov rsp, rbp\n");
    fprintf(out, "\tpop rbp\n");
    fprintf(out, "\tret\n");
}

void gen_program(AST *ast, FILE *out)
{
    if (!ast || !ast->lhs)
    {
        fprintf(stderr, "[CODEGEN ERROR] No functions found\n");
        exit(1);
    }

    if (!has_function(ast, "main"))
    {
        fprintf(stderr, "[CODEGEN ERROR] main not found\n");
        exit(1);
    }

    fprintf(out, "global _start\n");
    fprintf(out, "section .text\n\n");

    for (AST *func = ast->lhs; func; func = func->next)
        gen_function(func, out);

    fprintf(out, "_start:\n");
    fprintf(out, "\tmov rdi, [rsp]\n");
    fprintf(out, "\tlea rsi, [rsp+8]\n");
    fprintf(out, "\tsub rsp, 8\n");
    fprintf(out, "\tcall main\n");
    fprintf(out, "\tadd rsp, 8\n");
    fprintf(out, "\tmov rdi, rax\n");
    fprintf(out, "\tmov rax, 60\n");
    fprintf(out, "\tsyscall\n");
}
