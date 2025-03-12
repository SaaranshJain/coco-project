#include "parser.h"
#include "grammar.h"
#include "lexer.h"
#include "firstAndFollow.h"
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
        firstAndFollowSets[i]->firstSetEpsilon = false;
        firstAndFollowSets[i]->followSetSize = 0;
        firstAndFollowSets[i]->followSetDollar = false;

        // Initial allocation for sets (dynamic resizing later)
        int initialCapacity = 5;
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

    // Compute FIRST sets for all non-terminals
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        computeFirstSet(G, firstAndFollowSets, (enum NON_TERMINAL)(i + NUM_TERMINALS));
    }

    // Compute FOLLOW sets for all non-terminals
    computeFollowSet(G, firstAndFollowSets, G->startSymbol);
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        computeFollowSet(G, firstAndFollowSets, (enum NON_TERMINAL)(i + NUM_TERMINALS));
    }

    return firstAndFollowSets;
}

void createParseTable(FirstAndFollow *F, ParseTable T, Grammar G) {
    int NON_TERMINAL_OFFSET = 59; // First non-terminal index

    // init table
    for (int i = 0; i < NUM_NON_TERMINALS; i++) {
        for (int j = 0; j < NUM_TERMINALS; j++) {
            T[i * NUM_TERMINALS + j] = NULL;
        }
    }

    for (int i = 0; i < G->numRules; i++) {
        Rule rule = G->rules[i];
        int row_index = rule->lhs - NON_TERMINAL_OFFSET; // Normalize non-terminal index

        // first set
        for (int j = 0; j < F[row_index]->firstSetSize; j++) {
            enum TOKEN_TYPE terminal = F[row_index]->firstSet[j];
            T[row_index * NUM_TERMINALS + terminal] = rule; 
        }
        
        // epsilon case using follow set
        if (rule->rhsLength == 1 && rule->rhs[0]->isEpsilon) {
            for (int j = 0; j < F[row_index]->followSetSize; j++) {
                enum TOKEN_TYPE terminal = F[row_index]->followSet[j];
                T[row_index * NUM_TERMINALS + terminal] = rule;
            }
        }
    }
}

ParseTree parseInputSourceCode(char *testcaseFileName, ParseTable T) {
    FILE *testcasefile = fopen(testcaseFileName, "r");
    TwinBuffer buffer = getStream(testcasefile);
    TokenInfo token;
    LookupTable lt = create_lookup_table();

    while ((token = getNextToken(buffer, lt)) != NULL) {
    }
}

void printParseTree(ParseTree PT, char *outfile) {
}
