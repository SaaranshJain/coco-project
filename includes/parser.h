#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

FirstAndFollow *ComputeFirstAndFollowSets(Grammar G);
void createParseTable(FirstAndFollow *F, ParseTable T, Grammar G);
ParseTree parseInputSourceCode(char *testcaseFileName, ParseTable T);
void printParseTree(ParseTree PT, char *outfile);

#endif
