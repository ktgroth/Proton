
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/parser.h"
#include "include/print.h"

char *read_program(char *filename) {
    FILE *fp = fopen(filename, "r");

    char *program = (char*)calloc(1, sizeof(char)), *line = NULL;
    size_t line_size;
    while (getline(&line, &line_size, fp) != -1) {
        program = realloc(program, (strlen(program) + strlen(line) + 1) * sizeof(char));
        program = strcat(program, line);
    }
    free(line);
    fclose(fp);

    return program;
}

int main(int argc, char *argv[]) {
    char *input_filename = NULL, *final_filename = NULL;
    if (argc > 1)
        input_filename = argv[1];
    else {
        fprintf(stderr, "\033[0;31m");
        fprintf(stderr, "[ERROR] No input file was given\n");
        exit(1);
    }
    
    for (int i = 2; i < argc; ++i) {
        if (!strncmp(argv[i], "-", 1)) {
            if (!strcmp(argv[i] + 1, "o")) {
                if (argc > i + 1)
                    final_filename = argv[++i];
                else {
                    fprintf(stderr, "\033[0;31m");
                    fprintf(stderr, "[ERROR] No output file was given\n");
                    exit(1);
                }
            }
        }
    }

    char *program = read_program(input_filename);
    printf("%s\n", program);

    lexer_t lex = init_lexer(program);
    free(program);

    deque_t tokens = shunt(lex);
    
    printf("\n\nOutput:\n");
    print_deque(tokens);

    if (!final_filename) {
        final_filename = "a";
    }

    FILE *output = fopen(final_filename, "w");
    fwrite("Hello, World!\n", strlen("Hello, World!\n"), sizeof(char), output);

    return 0;
}
