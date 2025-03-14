#ifndef FIRSTANDFOLLOW_H
#define FIRSTANDFOLLOW_H

#include "parserDef.h"

uint64_t firstSetOfRule(int ruleind, Grammar G, uint64_t *memo);
uint64_t *computeFirstSets(FirstAndFollow *ff, Grammar G);
uint64_t followSetOfNT(FirstAndFollow *ff, Grammar G, enum NON_TERMINAL nt);
void computeFollowSets(FirstAndFollow *ff, Grammar G);

#endif
