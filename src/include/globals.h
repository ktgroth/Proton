
#ifndef PROTON_GLOBALS
#define PROTON_GLOBALS

#include "../../libc/include/hashmap.h"
#include "../../libc/include/trie.h"

trie_t *init_registers();
trie_t *init_keywords();
hashmap_t *init_symbols();
hashmap_t *init_ops();

extern trie_t *registers;
extern trie_t *keywords;
extern hashmap_t *symbols;
extern hashmap_t *ops;

#endif
