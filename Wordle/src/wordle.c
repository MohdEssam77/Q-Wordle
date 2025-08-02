#include "wordle.h"

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trie.h"

#define MAX_WORDS 100000

// Loads words from a file into a trie and randomly selects a word using drand48 function
Trie *generateDict(const char *filename, int k, char *selected1, char *selected2) {
    Trie *dict = create();
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open dictionary file");
        exit(EXIT_FAILURE);
    }

    char word[k + 2];  // +1 for newline, +1 for null
    char *words[MAX_WORDS];
    int count = 0;

    while (fgets(word, sizeof(word), file)) {
        size_t len = strlen(word);
        if (word[len - 1] == '\n') word[len - 1] = '\0';

        if ((int)strlen(word) != k) continue;

        insert(dict, word);

        if (count < MAX_WORDS) {
            words[count] = strdup(word);
            count++;
        }
    }
    fclose(file);

    if (count == 0) {
        fprintf(stderr, "No valid words in dictionary\n");
        exit(EXIT_FAILURE);
    }

    // Choose first word
    int index1 = (int)(drand48() * count);
    strcpy(selected1, words[index1]);

    // Choose second word for quantum
    if (selected2) {
        bool valid = false;
        for (int attempts = 0; attempts < count * 2; ++attempts) {
            int index2 = (int)(drand48() * count);
            bool disjoint = true;
            for (int i = 0; i < k && disjoint; ++i) {
                for (int j = 0; j < k; ++j) {
                    if (selected1[i] == words[index2][j]) {
                        disjoint = false;
                        break;
                    }
                }
            }
            if (disjoint) {
                strcpy(selected2, words[index2]);
                valid = true;
                break;
            }
        }
        if (!valid) {
            fprintf(stderr, "Failed to find disjoint word for quantum Wordle\n");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < count; ++i) {
        free(words[i]);
    }

    return dict;
}

// Prompts user input until a valid word is entered.
char *guess(Trie *dict, int k) {
    char buffer[128];
    printf("Please input your guess: ");

    while (true) {
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            fprintf(stderr, "Error reading input\n");
            exit(EXIT_FAILURE);
        }

        size_t len = strlen(buffer);
        if (buffer[len - 1] == '\n') buffer[len - 1] = '\0';

        // Convert to lowercase
        for (int i = 0; i < k && buffer[i]; ++i) buffer[i] = tolower(buffer[i]);

        if ((int)strlen(buffer) != k || (dict && !lookup(dict, buffer))) {
            printf("Invalid word. Try again: ");
        } else {
            char *result = malloc(k + 1);
            strcpy(result, buffer);
            return result;
        }
    }
}

// Returns array of feedback.
// fb variable is the array of feedback that will be returned.
feedback_result *getFeedback(const char *guess, const char *word1, const char *word2, int k) {
    feedback_result *fb = malloc(k * sizeof(feedback_result));

    int count1[26] = {0};  // unmatched letters from word1
    int count2[26] = {0};  // unmatched letters from word2
    int i;
    bool quantum = (word2 != NULL);
    // First pass: mark correct matches and count unmatched letters
    for (i = 0; i < k; ++i) {
        if (quantum) {
            if (guess[i] == word1[i]) {
                fb[i] = QUANTUMCORRECT;
            } else if (guess[i] == word2[i]) {
                fb[i] = QUANTUMCORRECT;
            } else {
                fb[i] = WRONG;
                count1[word1[i] - 'a']++;
                count2[word2[i] - 'a']++;
            }
        } else {
            if (guess[i] == word1[i]) {
                fb[i] = CORRECT;
            } else {
                fb[i] = WRONG;
                count1[word1[i] - 'a']++;
            }
        }
    }
    // Second pass: assign wrong position matches
    for (i = 0; i < k; ++i) {
        if (fb[i] != WRONG) continue;
        int c = guess[i] - 'a';
        if (count1[c] > 0) {
            fb[i] = quantum ? QUANTUMWRONGPOS : WRONGPOS;
            count1[c]--;
        } else if (quantum && count2[c] > 0) {
            fb[i] = QUANTUMWRONGPOS;
            count2[c]--;
        }
    }
    return fb;
}

// Prints emojis for feedback
void printFeedback(feedback_result *fb, int k) {
    printf("Result: ");
    for (int i = 0; i < k; ++i) {
        switch (fb[i]) {
            case CORRECT:
                printf("\xF0\x9F\x9F\xA9");
                break;  // 🟩
            case WRONGPOS:
                printf("\xF0\x9F\x9F\xA8");
                break;  // 🟨
            case WRONG:
                printf("\xE2\xAC\x9B");
                break;  // ⬛
            case QUANTUMCORRECT:
                printf("\xF0\x9F\x9F\xA2");
                break;  // 🟢
            case QUANTUMWRONGPOS:
                printf("\xF0\x9F\x9F\xA1");
                break;  // 🟡
        }
    }
    printf("\n");
}

// Determines if all letters are CORRECT
bool checkWin(const feedback_result *feedback, int k) {
    if (feedback == NULL) return false;
    for (int i = 0; i < k; i++) {
        if (feedback[i] != CORRECT) {
            return false;
        }
    }
    return true;
}
