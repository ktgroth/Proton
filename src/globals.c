
#include "include/globals.h"

trie_t *registers = NULL;
trie_t *keywords = NULL;
hashmap_t *symbols = NULL;
hashmap_t *ops = NULL;

trie_t *init_registers()
{
    registers = init_trie(0);
    trie_add(registers, "rax");
    trie_add(registers, "rbx");
    trie_add(registers, "rcx");
    trie_add(registers, "rdx");
    trie_add(registers, "rbp");
    trie_add(registers, "rsp");
    trie_add(registers, "rsi");
    trie_add(registers, "rdi");
    trie_add(registers, "r8");
    trie_add(registers, "r9");
    trie_add(registers, "r10");
    trie_add(registers, "r11");
    trie_add(registers, "r12");
    trie_add(registers, "r13");
    trie_add(registers, "r14");
    trie_add(registers, "r15");

    return registers;
}

trie_t *init_keywords()
{
    keywords = init_trie(0);
    trie_add(keywords, "call");
    trie_add(keywords, "syscall");
    trie_add(keywords, "ret");

    return keywords;
}

hashmap_t *init_symbols()
{
    symbols = init_hashmap(50);
    return symbols;
}

hashmap_t *init_ops()
{
    ops = init_hashmap(50);
    hashmap_add(ops, "+", 1);
    hashmap_add(ops, "-", 1);
    hashmap_add(ops, "*", 2);
    hashmap_add(ops, "/", 2);
    hashmap_add(ops, "&", 3);
    hashmap_add(ops, "|", 3);
    hashmap_add(ops, "&&", 4);
    hashmap_add(ops, "||", 4);


    return ops;
}
