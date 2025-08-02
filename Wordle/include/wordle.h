#ifndef WORDLE_H
#define WORDLE_H

#include "dict.h"
typedef enum { CORRECT, WRONGPOS, WRONG, QUANTUMCORRECT, QUANTUMWRONGPOS } feedback_result;
// Load dictionary and choose one or two words at random
Trie *generateDict(const char *filename, int k, char *selected1, char *selected2);

// Prompt the user to enter a guess
char *guess(Trie *dict, int k);

// Generate feedback for the guess
feedback_result *getFeedback(const char *guess, const char *word1, const char *word2, int k);

// Print visual emoji feedback
void printFeedback(feedback_result *fb, int k);

// Check whether all characters were correct
bool checkWin(const feedback_result *fb, int k);

#endif  // WORDLE_H
