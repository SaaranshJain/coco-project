#include "../includes/parser.h"
#include "../includes/grammar.h"
#include "../includes/lexer.h"
#include "../includes/firstAndFollow.h"
#include <stdlib.h>

FirstAndFollow *ComputeFirstAndFollowSets(Grammar G) {
    FirstAndFollow *firstAndFollowSets = (FirstAndFollow *)malloc(NUM_NON_TERMINALS * sizeof(FirstAndFollow));
    if (!firstAndFollowSets) {
        perror("Memory allocation failed");
        return NULL;
    }

    // Initialize First and Follow Sets for each Non-Terminal
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        firstAndFollowSets[i] = (FirstAndFollow)malloc(sizeof(struct firstAndFollow));
        if (!firstAndFollowSets[i]) {
            perror("Memory allocation failed");
            // Clean up previously allocated memory
            for (int j = 0; j < i; j++) {
                free(firstAndFollowSets[j]);
            }
            free(firstAndFollowSets);
            return NULL;
        }

        firstAndFollowSets[i]->nonTerminal = (enum NON_TERMINAL)(i + NUM_TERMINALS);
        firstAndFollowSets[i]->firstSetSize = 0;
        firstAndFollowSets[i]->firstSetCapacity = 1;
        firstAndFollowSets[i]->firstSetEpsilon = false;
        firstAndFollowSets[i]->followSetSize = 0;
        firstAndFollowSets[i]->followSetCapacity = 1;
        firstAndFollowSets[i]->followSetDollar = false;

        // Initial allocation for sets (dynamic resizing later)
        int initialCapacity = 1;
        firstAndFollowSets[i]->firstSet = (enum TOKEN_TYPE *)malloc(initialCapacity * sizeof(enum TOKEN_TYPE));
        if (!firstAndFollowSets[i]->firstSet) {
            perror("Memory allocation failed");
            // Clean up previously allocated memory
            for (int j = 0; j < NUM_NON_TERMINALS; j++) {
                if (firstAndFollowSets[j]->firstSet)
                    free(firstAndFollowSets[j]->firstSet);
                free(firstAndFollowSets[j]);
            }
            free(firstAndFollowSets);
            return NULL;
        }
        firstAndFollowSets[i]->followSet = (enum TOKEN_TYPE *)malloc(initialCapacity * sizeof(enum TOKEN_TYPE));
        if (!firstAndFollowSets[i]->followSet) {
            perror("Memory allocation failed");
            // Clean up previously allocated memory
            for (int j = 0; j < NUM_NON_TERMINALS; j++) {
                if (firstAndFollowSets[j]->firstSet)
                    free(firstAndFollowSets[j]->firstSet);
                if (firstAndFollowSets[j]->followSet)
                    free(firstAndFollowSets[j]->followSet);
                free(firstAndFollowSets[j]);
            }
            free(firstAndFollowSets);
            return NULL;
        }
    }

    // A dp boolean array to check the completeness of the first or follow set of a non-terminal
    bool* memoized = (bool*)malloc(NUM_NON_TERMINALS * sizeof(bool));
    if (!memoized) {
        perror("Memory allocation failed");
        // Clean up previously allocated memory
        for (int j = 0; j < NUM_NON_TERMINALS; j++) {
            free(firstAndFollowSets[j]->firstSet);
            free(firstAndFollowSets[j]->followSet);
            free(firstAndFollowSets[j]);
        }
        free(firstAndFollowSets);
        return NULL;
    }

    // Initializing all to False
    for (int i = 0; i < NUM_NON_TERMINALS; i++) { 
        memoized[i] = false;
    }

    // Compute FIRST sets for all non-terminals
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        computeFirstSet(G, firstAndFollowSets, (enum NON_TERMINAL)(i + NUM_TERMINALS), memoized);
    }

    // Again initializing all to False for follow set computation
    for (int i = 0; i < NUM_NON_TERMINALS; i++) { 
        memoized[i] = false; 
    }

    // Compute FOLLOW sets for all non-terminals
    computeFollowSet(G, firstAndFollowSets, G->startSymbol, memoized);
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        computeFollowSet(G, firstAndFollowSets, (enum NON_TERMINAL)(i + NUM_TERMINALS), memoized);
    }

    free(memoized);

    return firstAndFollowSets;
}



// Testing and Verifying
void printFirstAndFollowSets(Grammar G) {

    // Open the file for writing
    FILE* file = fopen("firstAndFollowTest.txt", "w");
    if (!file) {
        perror("Failed to open file");
        return;
    }
    printf("File Opened Successfully\n");

    FirstAndFollow* sets = ComputeFirstAndFollowSets(G);    
    printf("Sets Created Successfully");

    for (int i = 0; i < NT_A + 1; i++) {
        fprintf(file, "Non-terminal: %d\n", i);

        fprintf(file, "FIRST set: ");
        for (int j = 0; j < sets[i]->firstSetSize; j++) {
            fprintf(file, "%d ", sets[i]->firstSet[j]);
        }
        if (sets[i]->firstSetEpsilon) {
            fprintf(file, "ε");
        }
        fprintf(file, "\n");

        // fprintf(file, "FOLLOW set: ");
        // for (int j = 0; j < sets[i]->followSetSize; j++) {
        //     fprintf(file, "%d ", sets[i]->followSet[j]);
        // }
        // if (sets[i]->followSetDollar) {
        //     fprintf(file, "$");
        // }
        // fprintf(file, "\n\n");
    }

    fclose(file); // Close the file

    // Clean up
    for (int i = 0; i < NT_A + 1; i++) {
        free(sets[i]->firstSet);
        free(sets[i]->followSet);
        free(sets[i]);
    }
    free(sets);
}

int main(){
    printFirstAndFollowSets(&languageGrammar);
}