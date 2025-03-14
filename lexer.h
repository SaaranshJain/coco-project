#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

extern char *TOKEN_NAME_FROM_VALUE[NUM_TERMINALS];

TwinBuffer getStream(FILE *fp);
TokenInfo getNextToken(TwinBuffer B, LookupTable lt);
void removeComments(char *testcaseFile, char *cleanFile);

#endif
