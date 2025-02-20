#ifndef PARSER_H
#define PARSER_H

#include "parserDef.h"

FirstAndFollow* ComputeFirstAndFollowSets (Grammar G);

void createParseTable(FirstAndFollow F, Table T);

ParseTree parseInputSourceCode(char *testcaseFile, Table T);

void printParseTree(ParseTree PT, char *outfile);

#endif
