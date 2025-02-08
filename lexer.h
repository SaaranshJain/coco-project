#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

TwinBuffer getStream(FILE *fp);
TokenInfo getNextToken(TwinBuffer B); 
void removeComments(char *testcaseFile, char *cleanFile);

#endif
