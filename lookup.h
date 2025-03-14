/*
    Group number 1
    Saaransh Jain - 2022A7PS0074P
    Aman Patel - 2022A7PS0152P
    Vishnu Hari - 2022A7TS0094P
    Parth Sudan - 2022A7PS0177P
*/
#ifndef LOOKUP_H
#define LOOKUP_H

#include "lexerDef.h"

LookupTableNode create_node(char *lexeme, enum TOKEN_TYPE token, int line);
LookupTable create_lookup_table();
int hash(LookupTable lt, char *lexeme, int lexemeLength);
void insert(LookupTable lt, char *lexeme, int lexemeLength, enum TOKEN_TYPE token, int line);
void del(LookupTable lt, char *lexeme, int lexemeLength);
LookupTableNode search(LookupTable lt, char *lexeme, int lexemeLength);
void insert_keywords(LookupTable lt);

#endif
