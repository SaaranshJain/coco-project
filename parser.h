/*
    Group number 1
    Saaransh Jain - 2022A7PS0074P
    Aman Patel - 2022A7PS0152P
    Vishnu Hari - 2022A7TS0094P
    Parth Sudan - 2022A7PS0177P
*/
#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

extern char *NONTERMINAL_NAME_FROM_VALUE[NUM_NON_TERMINALS];

uint64_t *computeFirstAndFollowSets(FirstAndFollow *F, Grammar G);
ParseTable createParseTable(FirstAndFollow *F, Grammar G, uint64_t *memo);
ParseTree parseInputSourceCode(char *testcaseFileName, ParseTable T, Grammar G);
void printParseTree(ParseTree PT, char *outfile);

#endif
