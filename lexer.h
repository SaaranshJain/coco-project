/*
    Group number 1
    Saaransh Jain - 2022A7PS0074P
    Aman Patel - 2022A7PS0152P
    Vishnu Hari - 2022A7TS0094P
    Parth Sudan - 2022A7PS0177P
*/
#ifndef LEXER_H
#define LEXER_H

#include "lexerDef.h"

extern char *TOKEN_NAME_FROM_VALUE[NUM_TERMINALS];

TwinBuffer getStream(FILE *fp);
TokenInfo getNextToken(TwinBuffer B, LookupTable lt);
void removeComments(char *testcaseFile, char *cleanFile);

#endif
