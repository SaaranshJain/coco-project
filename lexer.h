#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

FILE *getStream(FILE *fp);
tokenInfo getNextToken(TwinBuffer B); 
void removeComments(char *testcaseFile, char *cleanFile);

#endif
