#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

TwinBuffer createTwinBuffer(FILE *fp);
FILE *getStream(FILE *fp);
TokenInfo getNextToken(TwinBuffer B); 
void removeComments(char *testcaseFile, char *cleanFile);

#endif
