#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

uint64_t *computeFirstAndFollowSets(FirstAndFollow *F, Grammar G);
ParseTable createParseTable(FirstAndFollow *F, Grammar G, uint64_t* memo);
ParseTree parseInputSourceCode(char *testcaseFileName, ParseTable T, Grammar G);
void printParseTree(ParseTree PT, char *outfile);

#endif
