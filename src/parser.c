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

int computeTokenCount(FirstAndFollow *F) {
    int maxToken = 0;
    for (int i = 0; i < NT_A; i++) {
        for (int j = 0; j < F[i]->firstSetSize; j++) {
            if (F[i]->firstSet[j] > maxToken) {
                maxToken = F[i]->firstSet[j];
            }
        }
        for (int j = 0; j < F[i]->followSetSize; j++) {
            if (F[i]->followSet[j] > maxToken) {
                maxToken = F[i]->followSet[j];
            }
        }
    }
    return maxToken + 1; // count = max index + 1
}

void createParseTable(FirstAndFollow *F, ParseTable T, Grammar G) {
    int TOKEN_COUNT = computeTokenCount(F);
    int TOKEN_DOLLAR_INDEX = TOKEN_COUNT - 1;

    // init empty table
    for (int i = 0; i < NT_A; i++) {
        for (int j = 0; j < TOKEN_COUNT; j++) {
            T[i * TOKEN_COUNT + j] = NULL;
        }
    }

    for (int i = 0; i < G->numRules; i++) {
        Rule rule = G->rules[i];
        enum NON_TERMINAL lhs = rule->lhs;

        for (int j = 0; j < F[lhs]->firstSetSize; j++) {
            enum TOKEN_TYPE terminal = F[lhs]->firstSet[j];
            T[lhs * TOKEN_COUNT + terminal] = rule;
        }

        // Handle epsilon using follow
        if (rule->rhsLength == 1 && rule->rhs[0]->isEpsilon) {
            for (int j = 0; j < F[lhs]->followSetSize; j++) {
                enum TOKEN_TYPE terminal = F[lhs]->followSet[j];
                T[lhs * TOKEN_COUNT + terminal] = rule;
            }

            if (F[lhs]->followSetDollar) {
                T[lhs * TOKEN_COUNT + TOKEN_DOLLAR_INDEX] = rule;
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
