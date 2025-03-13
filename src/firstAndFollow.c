#include "firstAndFollow.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint64_t firstSetOfRule(int ruleind, Grammar G, uint64_t *memo) {
    Rule rule = G->rules[ruleind];

    if (rule->isEpsilon) return 1;
    if (memo[ruleind] != 0) return memo[ruleind];

    uint64_t fs = 0;
    RuleRightSideElement *rhs = rule->rhs;

    bool isEps = true;

    for (int i = 0; i < rule->rhsLength; ++i) {
        uint64_t temp = 0;
        if (rhs[i]->isTerminal) {
            temp |= (1 << rhs[i]->symbol.terminal);
            isEps = false;
            fs |= (temp >> 1) << 1;
            break;
        }

        for (int j = 0; j < G->numRules; ++j) {
            if (G->rules[j]->lhs == rhs[i]->symbol.nonTerminal) {
                temp |= firstSetOfRule(j, G, memo);
            }
        }

        isEps = isEps && (temp % 2 == 1);
        fs |= (temp >> 1) << 1;

        if (temp % 2 == 0) {
            break;
        }
    }

    if (isEps) {
        fs = fs | 1;
    }

    return memo[ruleind] = fs;
}

uint64_t *computeFirstSets(FirstAndFollow *ff, Grammar G) {
    uint64_t *memo = (uint64_t *)calloc(G->numRules, sizeof(uint16_t));

    for (int i = 0; i < G->numRules; ++i) {
        ff[G->rules[i]->lhs - NUM_TERMINALS]->firstSet |= firstSetOfRule(i, G, memo);
    }

    return memo;
}

uint64_t followSetOfNT(FirstAndFollow *ff, Grammar G, enum NON_TERMINAL nt) {
    int ffind = nt - NUM_TERMINALS;

    if (ff[nt - NUM_TERMINALS]->followSet != 0)
        return ff[nt - NUM_TERMINALS]->followSet;

    if (nt == G->startSymbol) {
        ff[nt - NUM_TERMINALS]->followSet = 1;
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
                    firstSetOfRemaining |= (1 << rule->rhs[k]->symbol.terminal);
                    break;
                }

                firstSetOfRemaining |= 2 * (ff[ffind]->firstSet / 2);
                isEps = isEps && (ff[ffind]->firstSet % 2 == 1);

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
    for (int i = NUM_TERMINALS; i < NUM_TERMINALS + NUM_NON_TERMINALS; ++i) {
        followSetOfNT(ff, G, i);
    }
}
