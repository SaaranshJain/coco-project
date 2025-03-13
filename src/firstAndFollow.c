#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../includes/firstAndFollow.h"

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
    if (*setSize > 0 && isTerminalInSet(terminal, *set, *setSize)) {
        return 0; // Already in the set
    }
    printf("Reached here..\n");
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

void computeFirstSet(Grammar G, FirstAndFollow *firstAndFollowSets, enum NON_TERMINAL nonTerminal, bool* memoized) {

    if (memoized[nonTerminal-NUM_TERMINALS]) return; // Already computed

    printf("Computing First set...\n");
    for (int i = 0; i < G->numRules; i++) {
        if (G->rules[i]->lhs == nonTerminal) {
            // Iterating through RHS of the rule
            for (int j = 0; j < G->rules[i]->rhsLength; j++) {
                if (G->rules[i]->rhs[j]->isTerminal) {
                    // If it's a terminal, add it to the FIRST set
                    printf("Adding Terminal...\n");
                    int result = addTerminalToSet(G->rules[i]->rhs[j]->symbol.terminal,
                                                  &firstAndFollowSets[nonTerminal]->firstSet,
                                                  &firstAndFollowSets[nonTerminal]->firstSetSize,
                                                  &firstAndFollowSets[nonTerminal]->firstSetCapacity);
                    printf("Added to Set..\n");
                    if (result == -1)
                        exit(EXIT_FAILURE); // Handle memory allocation failure
                    break;                  // Move to the next rule
                } else {
                    // If it's a non-terminal, add FIRST(non-terminal) to FIRST(nonTerminal)
                    enum NON_TERMINAL currentNonTerminal = G->rules[i]->rhs[j]->symbol.nonTerminal;
                    
                    // If it's FIRST set is not complete, recursively compute its FIRST set
                    if(!memoized[currentNonTerminal-NUM_TERMINALS]){
                        printf("Going to Recursion..\n");
                        computeFirstSet(G, firstAndFollowSets, currentNonTerminal, memoized);
                    }

                    for (int k = 0; k < firstAndFollowSets[currentNonTerminal]->firstSetSize; k++) {
                        int result = addTerminalToSet(firstAndFollowSets[currentNonTerminal]->firstSet[k],
                                                      &firstAndFollowSets[nonTerminal]->firstSet,
                                                      &firstAndFollowSets[nonTerminal]->firstSetSize,
                                                      &firstAndFollowSets[nonTerminal]->firstSetCapacity);
                        if (result == -1)
                            exit(EXIT_FAILURE); // Handle memory allocation failure
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

    memoized[nonTerminal-NUM_TERMINALS] = true; // Mark as computed
}

void computeFollowSet(Grammar G, FirstAndFollow* firstAndFollowSets, enum NON_TERMINAL nonTerminal, bool* memoized) {
    
    if (memoized[nonTerminal-NUM_TERMINALS]) return; // Already computed

    if (nonTerminal == G->startSymbol){
        int result = addTerminalToSet(TK_DOLLAR,
                                      &firstAndFollowSets[nonTerminal]->followSet,
                                      &firstAndFollowSets[nonTerminal]->followSetSize,
                                      &firstAndFollowSets[nonTerminal]->followSetCapacity);
         if (result == -1) exit(EXIT_FAILURE); // Handle memory allocation failure
        firstAndFollowSets[nonTerminal]->followSetDollar = true;
    }

    for (int i = 0; i < G->numRules; i++) {
        for (int j = 0; j < G->rules[i]->rhsLength; j++) {
            if (!G->rules[i]->rhs[j]->isTerminal && G->rules[i]->rhs[j]->symbol.nonTerminal == nonTerminal) {
                // Rule: A -> alpha B beta
                if (j < G->rules[i]->rhsLength - 1) {
                    // beta is not empty
                    int k = j + 1; // Start of beta
                    bool addFollowOfA = false;

                    while (k < G->rules[i]->rhsLength) {
                        if (G->rules[i]->rhs[k]->isTerminal) {
                            // beta starts with a terminal
                            int result = addTerminalToSet(G->rules[i]->rhs[k]->symbol.terminal,
                                                          &firstAndFollowSets[nonTerminal]->followSet,
                                                          &firstAndFollowSets[nonTerminal]->followSetSize,
                                                          &firstAndFollowSets[nonTerminal]->followSetCapacity);
                             if (result == -1) exit(EXIT_FAILURE); // Handle memory allocation failure
                            break; // Exit loop since we've found a terminal
                        } else {
                            // beta starts with a non-terminal
                            enum NON_TERMINAL nextNonTerminal = G->rules[i]->rhs[k]->symbol.nonTerminal;

                            for (int index = 0; index < firstAndFollowSets[nextNonTerminal]->firstSetSize; index++) {
                                int result = addTerminalToSet(firstAndFollowSets[nextNonTerminal]->firstSet[index],
                                                              &firstAndFollowSets[nonTerminal]->followSet,
                                                              &firstAndFollowSets[nonTerminal]->followSetSize,
                                                              &firstAndFollowSets[nonTerminal]->followSetCapacity);
                                 if (result == -1) exit(EXIT_FAILURE); // Handle memory allocation failure
                            }

                            if (!firstAndFollowSets[nextNonTerminal]->firstSetEpsilon) {
                                break; // Exit loop since epsilon is not in FIRST(nextNonTerminal)
                            }
                            k++; // Move to the next symbol in beta
                        }
                    }

                    // Reached the end of beta without finding a terminal or a non-terminal without epsilon
                    if (k == G->rules[i]->rhsLength) {
                        // Add FOLLOW(A) to FOLLOW(B)
                        enum NON_TERMINAL A = G->rules[i]->lhs;
                        computeFollowSet(G, firstAndFollowSets, A, memoized); // Ensure FOLLOW is computed

                        for (int index = 0; index < firstAndFollowSets[A]->followSetSize; index++) {
                            int result = addTerminalToSet(firstAndFollowSets[A]->followSet[index],
                                                          &firstAndFollowSets[nonTerminal]->followSet,
                                                          &firstAndFollowSets[nonTerminal]->followSetSize,
                                                          &firstAndFollowSets[nonTerminal]->followSetCapacity);
                             if (result == -1) exit(EXIT_FAILURE); // Handle memory allocation failure
                        }
                        if(firstAndFollowSets[A]->followSetDollar){
                            firstAndFollowSets[nonTerminal]->followSetDollar = true;
                        }
                    }
                } else {
                    // Rule: A -> alpha B  (B is the last symbol)
                    // Add FOLLOW(A) to FOLLOW(B)
                    enum NON_TERMINAL A = G->rules[i]->lhs;
                    if(!memoized[A-NUM_TERMINALS]){
                        computeFollowSet(G, firstAndFollowSets, A, memoized); // Ensure FOLLOW is computed
                    }

                    for (int index = 0; index < firstAndFollowSets[A]->followSetSize; index++) {
                        int result = addTerminalToSet(firstAndFollowSets[A]->followSet[index],
                                                      &firstAndFollowSets[nonTerminal]->followSet,
                                                      &firstAndFollowSets[nonTerminal]->followSetSize,
                                                      &firstAndFollowSets[nonTerminal]->followSetCapacity);
                         if (result == -1) exit(EXIT_FAILURE); // Handle memory allocation failure
                    }
                    if(firstAndFollowSets[A]->followSetDollar){
                        firstAndFollowSets[nonTerminal]->followSetDollar = true;
                    }
                }
            }
        }
    }

    memoized[nonTerminal-NUM_TERMINALS] = true; // Mark as computed
}
