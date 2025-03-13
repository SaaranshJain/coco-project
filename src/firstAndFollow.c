#include "firstAndFollow.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Checks if a terminal is already present in a set
bool isTerminalInSet(enum TOKEN_TYPE terminal, enum TOKEN_TYPE *set, int setSize) {
    for (int i = 0; i < setSize; i++) {
        if (set[i] == terminal) {
            return true; // Found
        }
    }
    return false; // Not found
}

// Adds a terminal to a set, resizing if necessary
int addTerminalToSet(enum TOKEN_TYPE terminal, enum TOKEN_TYPE **set, int *setSize, int *capacity) {
    if (isTerminalInSet(terminal, *set, *setSize)) {
        return 0; // Already in the set
    }

    if (*setSize == *capacity) {
        // Resize the set
        *capacity *= 2;
        enum TOKEN_TYPE *newSet = (enum TOKEN_TYPE *)realloc(*set, *capacity * sizeof(enum TOKEN_TYPE));
        if (!newSet) {
            perror("Memory allocation failed");
            return -1; // Indicate failure
        }
        *set = newSet;
    }

    (*set)[*setSize] = terminal;
    (*setSize)++;
    return 1; // Added successfully
}

void computeFirstSet(Grammar G, FirstAndFollow *firstAndFollowSets, enum NON_TERMINAL nonTerminal) {
    for (int i = 0; i < G->numRules; i++) {
        if (G->rules[i]->lhs == nonTerminal) {
            // Iterating through RHS of the rule
            for (int j = 0; j < G->rules[i]->rhsLength; j++) {
                if (G->rules[i]->rhs[j]->isTerminal) {
                    // If it's a terminal, add it to the FIRST set
                    int result = addTerminalToSet(G->rules[i]->rhs[j]->symbol.terminal,
                                                  &firstAndFollowSets[nonTerminal]->firstSet,
                                                  &firstAndFollowSets[nonTerminal]->firstSetSize,
                                                  &firstAndFollowSets[nonTerminal]->firstSetSize);
                    if (result == -1)
                        exit(EXIT_FAILURE); // Handle memory allocation failure
                    break;                  // Move to the next rule
                } else {
                    // If it's a non-terminal, add FIRST(non-terminal) to FIRST(nonTerminal)
                    enum NON_TERMINAL currentNonTerminal = G->rules[i]->rhs[j]->symbol.nonTerminal;
                    int added = 0;

                    for (int k = 0; k < firstAndFollowSets[currentNonTerminal]->firstSetSize; k++) {
                        int result = addTerminalToSet(firstAndFollowSets[currentNonTerminal]->firstSet[k],
                                                      &firstAndFollowSets[nonTerminal]->firstSet,
                                                      &firstAndFollowSets[nonTerminal]->firstSetSize,
                                                      &firstAndFollowSets[nonTerminal]->firstSetSize);
                        if (result == -1)
                            exit(EXIT_FAILURE); // Handle memory allocation failure
                        added += result;
                    }

                    // If FIRST(currentNonTerminal) contains epsilon, continue to the next symbol on RHS
                    if (!firstAndFollowSets[currentNonTerminal]->firstSetEpsilon) {
                        break; // Stop if epsilon is not in FIRST(currentNonTerminal)
                    }
                    if (firstAndFollowSets[currentNonTerminal]->firstSetEpsilon && j == G->rules[i]->rhsLength - 1) {
                        firstAndFollowSets[nonTerminal]->firstSetEpsilon = true;
                    }
                }
            }
            if (G->rules[i]->rhsLength == 1 && G->rules[i]->rhs[0]->isEpsilon) {
                firstAndFollowSets[nonTerminal]->firstSetEpsilon = true;
            }
        }
    }
}

void computeFollowSet(Grammar G, FirstAndFollow *firstAndFollowSets, enum NON_TERMINAL nonTerminal) {

    if (nonTerminal == G->startSymbol) {
        int result = addTerminalToSet(TK_DOLLAR,
                                      &firstAndFollowSets[nonTerminal]->followSet,
                                      &firstAndFollowSets[nonTerminal]->followSetSize,
                                      &firstAndFollowSets[nonTerminal]->followSetSize);
        if (result == -1)
            exit(EXIT_FAILURE); // Handle memory allocation failure
        firstAndFollowSets[nonTerminal]->followSetDollar = true;
    }

    for (int i = 0; i < G->numRules; i++) {
        for (int j = 0; j < G->rules[i]->rhsLength; j++) {
            if (!G->rules[i]->rhs[j]->isTerminal && G->rules[i]->rhs[j]->symbol.nonTerminal == nonTerminal) {
                // Rule: A -> alpha B beta
                if (j < G->rules[i]->rhsLength - 1) {
                    // beta is not empty
                    if (G->rules[i]->rhs[j + 1]->isTerminal) {
                        // beta starts with a terminal
                        int result = addTerminalToSet(G->rules[i]->rhs[j + 1]->symbol.terminal,
                                                      &firstAndFollowSets[nonTerminal]->followSet,
                                                      &firstAndFollowSets[nonTerminal]->followSetSize,
                                                      &firstAndFollowSets[nonTerminal]->followSetSize);
                        if (result == -1)
                            exit(EXIT_FAILURE); // Handle memory allocation failure
                    } else {
                        // beta starts with a non-terminal
                        enum NON_TERMINAL nextNonTerminal = G->rules[i]->rhs[j + 1]->symbol.nonTerminal;
                        for (int k = 0; k < firstAndFollowSets[nextNonTerminal]->firstSetSize; k++) {
                            int result = addTerminalToSet(firstAndFollowSets[nextNonTerminal]->firstSet[k],
                                                          &firstAndFollowSets[nonTerminal]->followSet,
                                                          &firstAndFollowSets[nonTerminal]->followSetSize,
                                                          &firstAndFollowSets[nonTerminal]->followSetSize);
                            if (result == -1)
                                exit(EXIT_FAILURE); // Handle memory allocation failure
                        }
                        // If epsilon is in FIRST(beta), add FOLLOW(A) to FOLLOW(B)
                        if (firstAndFollowSets[nextNonTerminal]->firstSetEpsilon) {

                            enum NON_TERMINAL A = G->rules[i]->lhs;
                            for (int k = 0; k < firstAndFollowSets[A]->followSetSize; k++) {
                                int result = addTerminalToSet(firstAndFollowSets[A]->followSet[k],
                                                              &firstAndFollowSets[nonTerminal]->followSet,
                                                              &firstAndFollowSets[nonTerminal]->followSetSize,
                                                              &firstAndFollowSets[nonTerminal]->followSetSize);
                                if (result == -1)
                                    exit(EXIT_FAILURE); // Handle memory allocation failure
                            }
                            if (firstAndFollowSets[A]->followSetDollar) {
                                firstAndFollowSets[nonTerminal]->followSetDollar = true;
                            }
                        }
                    }
                } else {
                    // Rule: A -> alpha B  (B is the last symbol)
                    // Add FOLLOW(A) to FOLLOW(B)
                    enum NON_TERMINAL A = G->rules[i]->lhs;
                    for (int k = 0; k < firstAndFollowSets[A]->followSetSize; k++) {
                        int result = addTerminalToSet(firstAndFollowSets[A]->followSet[k],
                                                      &firstAndFollowSets[nonTerminal]->followSet,
                                                      &firstAndFollowSets[nonTerminal]->followSetSize,
                                                      &firstAndFollowSets[nonTerminal]->followSetSize);
                        if (result == -1)
                            exit(EXIT_FAILURE); // Handle memory allocation failure
                    }
                    if (firstAndFollowSets[A]->followSetDollar) {
                        firstAndFollowSets[nonTerminal]->followSetDollar = true;
                    }
                }
            }
        }
    }
}
