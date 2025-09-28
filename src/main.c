
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/lexer.h"


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

int main(int argc, const char *argv[])
{
    printf("%s\n", argv[1]);
    const char *program = read_program(argv[1]);
    lexer_t *lexer = init_lexer(program);

    token_t *token = NULL;
    while ((token = next_token(lexer))->type != TOKEN_EOF)
        print_token(token);
    print_token(token);

    return 0;
}

