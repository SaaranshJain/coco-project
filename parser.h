#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

FirstAndFollow* ComputeFirstAndFollowSets (Grammar G);

void createParseTable(FirstAndFollow* F, Table T, Grammar G);

ParseTree parseInputSourceCode(char *testcaseFileName, Table T);

void printParseTree(ParseTree PT, char *outfile);

#endif
