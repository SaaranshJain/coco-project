#include <stdlib.h>
#include <string.h>
#include "parserDef.h"
#include "parser.h"

int computeTokenCount (FirstAndFollow* F) {
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

void createParseTable (FirstAndFollow* F, Table T, Grammar G) {
    int TOKEN_COUNT = computeTokenCount(F);

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
                T[lhs * TOKEN_COUNT - 1] = rule;
            }
        }
    }
}