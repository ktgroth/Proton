
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/trie.h"

#define ALPHABET_SIZE 38
static const char *alphabet = "abcdefghijklmnopqrstuvwxyz0123456789_";

trie_t *init_trie(char value)
{
    trie_t *trie = (trie_t *)calloc(1, sizeof(trie_t));
    if (!trie)
    {
        perror("Could Not Allocate Trie ");
        return NULL;
    }

    trie->children = (trie_t **)calloc(ALPHABET_SIZE, sizeof(trie_t *));
    trie->is_terminal = 1;
    trie->value = value;

    return trie;
}

size_t get_index(char c)
{
    const char *ptr = strchr(alphabet, c);
    if (ptr)
        return ptr - alphabet;
    return -1;
}

void trie_add(trie_t *trie, char *str)
{
    for (size_t i = 0; i < strlen(str); ++i)
    {
        char c = str[i];
        size_t k = get_index(c);
        if (!trie->children[k])
            trie->children[k] = init_trie(c);
        trie = trie->children[k];
        trie->is_terminal = 0;
    }

    trie->is_terminal = 1;
}

trie_t *trie_remove(trie_t *trie, char *str)
{
    if (!trie)
        return NULL;

    if (!strlen(str))
    {
        if (trie->is_terminal) {
            trie->is_terminal = 0;
            trie->value = 0;
        }

        for (size_t i = 0; i < ALPHABET_SIZE; ++i)
        {
            if (trie->children[i])
                return trie;
        }

        return NULL;
    }

    size_t k = get_index(str[0]);
    trie->children[k] = trie_remove(trie->children[k], str + 1);
    return trie;
}

int search_trie(trie_t *trie, char *str)
{
    for (size_t i = 0; i < strlen(str); ++i)
    {
        size_t k = get_index(str[i]);
        if (!trie->children[k])
        {
            return 0;
        }
        trie = trie->children[k];
    }

    return 1;
}
