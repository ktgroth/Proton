
#ifndef LIBC_TRIE
#define LIBC_TRIE

typedef struct trie_t {
    struct trie_t **children;
    int is_terminal;
    char value;
} trie_t;

trie_t *init_trie(char value);

void trie_add(trie_t *trie, char *str);
trie_t *trie_remove(trie_t *trie, char *str);
int search_trie(trie_t *trie, char *str);

#endif
