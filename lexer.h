#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

TwinBuffer createTwinBuffer(FILE *fp);
TwinBuffer getStream(FILE *fp);
TokenInfo getNextToken(TwinBuffer B); 
void removeComments(char *testcaseFile, char *cleanFile);

#endif
