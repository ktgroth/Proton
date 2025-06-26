
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "include/first_pass.h"
#include "include/globals.h"

char *read_program(char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        perror("Could Open File ");
        return NULL;
    }

    char *program = (char *)calloc(1, sizeof(char));

    char *line;
    size_t n;
    while (getline(&line, &n, fp) != -1)
    {
        program = realloc(program, (strlen(program) + n) * sizeof(char));
        program = strcat(program, line);
    }
    fclose(fp);

    return program;
}

static size_t tabs = 0;

void print_ast(ast_t *ast) {
    for (size_t i = 0; i < tabs; ++i)
        printf("\t");
    
    if (!ast)
    {
        printf("(nul)\n");
        return;
    }
    
    printf("%s %d ", ast->name, ast->atype);
    switch (ast->type)
    {
        case TYPE_STRING:
        case TYPE_POINTER:
        case TYPE_OP:
            printf("%ld\n", ast->line);
            break;

        case TYPE_BINARY:
        case TYPE_INT:
        case TYPE_HEX:
        case TYPE_OCTAL:
            printf("%ld\n", ast->value_i);
            break;

        case TYPE_FLOAT:
            printf("%f\n", ast->value_f);
            break;

        default:
            printf("%ld\n", ast->line);
    }
    size_t s = ast->children->size;
    ++tabs;
    for (size_t i = 0; i < s; ++i)
        print_ast(ast->children->child[i]);
    --tabs;
}

int main(int argc, char *argv[])
{
    char *infile = NULL;
    char *outfile = NULL;

    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-o") && i + 1 < argc)
        {
            i += 1;
            outfile = argv[i];
        } else
        {
            infile = argv[i];
        }
    }

    if (!infile)
    {
        fprintf(stderr, "Usage: %s <inputfile> [-o outputfile]\n", argv[0]);
        return 1;
    }

    if (!outfile)
        outfile = "a.out";

    char *program = read_program(infile);
    if (!program)
        return 1;
        
    init_registers();
    init_keywords();
    init_symbols();
    init_ops();
    
    fp_parser_t *parser = init_fp_parser(program);
    if (!parser)
        return 1;

    ast_t *ast = parse_init(parser);
    print_ast(ast);

    return 0;
}
