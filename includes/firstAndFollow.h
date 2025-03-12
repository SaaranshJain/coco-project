#ifndef FIRSTANDFOLLOW_H
#define FIRSTANDFOLLOW_H

#include "parserDef.h"

bool isTerminalInSet(enum TOKEN_TYPE terminal, enum TOKEN_TYPE *set, int setSize);
int addTerminalToSet(enum TOKEN_TYPE terminal, enum TOKEN_TYPE **set, int *setSize, int *capacity);
void computeFirstSet(Grammar G, FirstAndFollow *firstAndFollowSets, enum NON_TERMINAL nonTerminal);
void computeFollowSet(Grammar G, FirstAndFollow *firstAndFollowSets, enum NON_TERMINAL nonTerminal);

#endif
