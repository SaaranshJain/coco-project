#include "firstAndFollow.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t firstSetOfRule(int ruleind, Grammar G, uint64_t *memo) {
    Rule rule = G->rules[ruleind];

    if (rule->isEpsilon) return memo[ruleind] = 1;
    if (memo[ruleind] != 0) return memo[ruleind];

    uint64_t fs = 0;
    RuleRightSideElement *rhs = rule->rhs;

    bool isEps = true;

    for (int i = 0; i < rule->rhsLength; ++i) {
        uint64_t temp = 0;
        if (rhs[i]->isTerminal) {
            temp |= (1UL << rhs[i]->symbol.terminal);
            isEps = false;
            fs |= temp;
            break;
        }

        for (int j = 0; j < G->numRules; ++j) {
            if (G->rules[j]->lhs == rhs[i]->symbol.nonTerminal) {
                temp |= firstSetOfRule(j, G, memo);
            }
        }

        isEps = isEps && (temp & 1);
        fs |= temp;

        if (!isEps) {
            break;
        }
    }

    if (isEps) {
        fs = fs | 1;
    } else {
        fs = fs & (((1UL << 63) - 1) << 1);
    }

    return memo[ruleind] = fs;
}

uint64_t *computeFirstSets(FirstAndFollow *ff, Grammar G) {
    uint64_t *memo = (uint64_t *)calloc(G->numRules, sizeof(uint64_t));

    for (enum NON_TERMINAL i = NUM_TERMINALS; i < NUM_TERMINALS + NUM_NON_TERMINALS; ++i) {
        for (int j = 0; j < G->numRules; ++j) {
            if (G->rules[j]->lhs == i) {
                ff[i - NUM_TERMINALS]->firstSet |= firstSetOfRule(j, G, memo);
            }
        }

        printf("%lu,\n", ff[i - NUM_TERMINALS]->firstSet);
    }

    return memo;
}

uint64_t followSetOfNT(FirstAndFollow *ff, Grammar G, enum NON_TERMINAL nt) {
    int ffind = nt - NUM_TERMINALS;

    if (ff[ffind]->followSet != 0)
        return ff[ffind]->followSet;

    if (nt == G->startSymbol) {
        ff[ffind]->followSet = 1;
    }

    for (int i = 0; i < G->numRules; ++i) {
        Rule rule = G->rules[i];

        for (int j = 0; j < rule->rhsLength; ++j) {
            RuleRightSideElement rhsEl = rule->rhs[j];
            if (rhsEl->isTerminal || rhsEl->symbol.nonTerminal != nt)
                continue;

            uint64_t firstSetOfRemaining = 0;
            bool isEps = true;

            for (int k = j + 1; k < rule->rhsLength; ++k) {
                if (rule->rhs[k]->isTerminal) {
                    firstSetOfRemaining |= (1UL << rule->rhs[k]->symbol.terminal);
                    isEps = false;
                    break;
                }

                firstSetOfRemaining |= (ff[rule->rhs[k]->symbol.nonTerminal - NUM_TERMINALS]->firstSet >> 1) << 1;
                isEps = isEps && (ff[rule->rhs[k]->symbol.nonTerminal - NUM_TERMINALS]->firstSet & 1);

                if (!isEps) {
                    break;
                }
            }

            ff[ffind]->followSet |= firstSetOfRemaining;

            if (isEps) {
                ff[nt - NUM_TERMINALS]->followSet |= followSetOfNT(ff, G, rule->lhs);
            }
        }
    }

    return ff[nt - NUM_TERMINALS]->followSet;
}

void computeFollowSets(FirstAndFollow *ff, Grammar G) {
    for (enum NON_TERMINAL i = NUM_TERMINALS; i < NUM_TERMINALS + NUM_NON_TERMINALS; ++i) {
        followSetOfNT(ff, G, i);
        // printf("Follow set of %i = %lu\n", i, followSetOfNT(ff, G, i));
    }
}
