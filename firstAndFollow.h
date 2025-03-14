/*
    Group number 1
    Saaransh Jain - 2022A7PS0074P
    Aman Patel - 2022A7PS0152P
    Vishnu Hari - 2022A7TS0094P
    Parth Sudan - 2022A7PS0177P
*/
#ifndef FIRSTANDFOLLOW_H
#define FIRSTANDFOLLOW_H

#include "parserDef.h"

uint64_t firstSetOfRule(int ruleind, Grammar G, uint64_t *memo);
uint64_t *computeFirstSets(FirstAndFollow *ff, Grammar G);
uint64_t followSetOfNT(FirstAndFollow *ff, Grammar G, enum NON_TERMINAL nt);
void computeFollowSets(FirstAndFollow *ff, Grammar G);

#endif
