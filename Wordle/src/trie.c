// trie.c
#include "trie.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dict.h"

#define ALPHABET_SIZE 26

/// Trie node
struct Trie {
    struct Trie *children[ALPHABET_SIZE];
    bool is_end_of_word;
};

// Creates a new Trie node
static Trie *createNode(void) {
    Trie *node = malloc(sizeof(Trie));
    if (!node) {
        perror("Failed to allocate Trie node");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < ALPHABET_SIZE; ++i) node->children[i] = NULL;
    node->is_end_of_word = false;
    return node;
}

Trie *create(void) { return createNode(); }

// Inserts a word into the trie
void insert(Trie *dict, char *str) {
    Trie *node = dict;
    for (int i = 0; str[i]; ++i) {
        if (str[i] < 'a' || str[i] > 'z') continue;
        int index = str[i] - 'a';
        if (!node->children[index]) node->children[index] = createNode();
        node = node->children[index];
    }
    node->is_end_of_word = true;
}

// Checks if a word exists.
bool lookup(Trie *dict, char *str) {
    Trie *node = dict;
    for (int i = 0; str[i]; ++i) {
        if (str[i] < 'a' || str[i] > 'z') return false;
        int index = str[i] - 'a';
        if (!node->children[index]) return false;
        node = node->children[index];
    }
    return node->is_end_of_word;
}

// Destroy all nodes in the trie
static void destroyNode(Trie *node) {
    if (!node) return;
    for (int i = 0; i < ALPHABET_SIZE; ++i) destroyNode(node->children[i]);
    free(node);
}

void destroy(Trie *dict) { destroyNode(dict); }
