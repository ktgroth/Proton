
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/parser.h"
#include "include/codegen.h"


const char *read_program(const char *filepath)
{
    FILE *fp = fopen(filepath, "r+");
    if (!fp)
    {
        perror("Could not read file!");
        return NULL;
    }

    char *program = (char *)calloc(1, sizeof(char));
    size_t program_size = 0;
    size_t used = 0;

    char *line = NULL;
    size_t n;
    ssize_t len;
    while ((len = getline(&line, &n, fp)) != -1)
    {
        size_t needed = used + len + 1;
        if (needed > program_size)
        {
            size_t new_size = needed * 2;
            char *tmp = realloc(program, new_size);
            if (!tmp)
            {
                perror("Realloc Failed");
                free(program);
                free(line);
                fclose(fp);
                return NULL;
            }

            program = tmp;
            program_size = new_size;
        }

        memcpy(program + used, line, len);
        used += len;
        program[used] = '\0';
    }

    free(line);
    fclose(fp);
    return program;
}

void ast_dump(AST *n, int depth)
{
    for (int i = 0; i < depth; ++i)
        printf(" ");
    
    switch (n->kind)
    {
        case AST_PROGRAM: printf("PROGRAM\n"); break;
        case AST_FUNCTION: printf("FUNCTION %s (stack=%d)\n", n->name, n->stack_size); break;
        case AST_BLOCK: printf("BLOCK\n"); break;
        case AST_NUM: printf("NUM %d\n", n->value); break;
        case AST_VAR: printf("VAR %s (offset=%d)\n", n->name, n->offset); break;
        case AST_CALL: printf("CALL %s\n", n->name); break;
        case AST_VAR_DECL: printf("VAR_DECL\n"); break;
        case AST_ASSIGN: printf("ASSIGN\n"); break;
        case AST_ADD: printf("ADD\n"); break;
        case AST_SUB: printf("SUB\n"); break;
        case AST_MUL: printf("MUL\n"); break;
        case AST_DIV: printf("DIV\n"); break;
        case AST_EQ: printf("EQ\n"); break;
        case AST_NE: printf("NE\n"); break;
        case AST_LT: printf("LT\n"); break;
        case AST_LE: printf("LE\n"); break;
        case AST_GT: printf("GT\n"); break;
        case AST_GE: printf("GE\n"); break;
        case AST_RETURN: printf("RETURN\n"); break;
        case AST_IF: printf("IF\n"); break;
    }

    if (n->lhs) ast_dump(n->lhs, depth + 1);
    if (n->rhs) ast_dump(n->rhs, depth + 1);
    if (n->else_branch)
    { 
        for (int i = 0; i < depth; ++i)
        printf(" ");
        printf("ELSE\n");
        ast_dump(n->else_branch, depth + 1);
    }
    if (n->next) ast_dump(n->next, depth);
}

int main(int argc, const char *argv[])
{
    const char *src = read_program(argv[1]);

    printf("%s\n\n", src);

    Lexer l;
    Parser p;

    init_lexer(&l, src);
    init_parser(&p, &l);

    // for (;;)
    // {
    //     Token t = lexer_next_token(l);
    //     printf("%s", token_name(t.kind));
    //
    //     if (t.kind == TOK_IDENT)
    //         printf("(%s)", t.lexeme);
    //     else if (t.kind == TOK_NUMBER)
    //         printf("(%d)", t.value);
    //
    //     printf("\n");
    //
    //     if (t.kind == TOK_EOF)
    //         break;
    // }


    AST *program = parse_program(&p);
    ast_dump(program, 0);

    FILE *out = fopen("out.s", "w");
    gen_program(program, out);
    fclose(out);

    system("nasm -felf64 out.s");
    system("gcc -nostdlib -no-pie out.o -o out");

    return 0;
}
