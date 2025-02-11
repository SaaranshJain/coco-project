#ifndef LOOKUP_H
#define LOOKUP_H

#include "lexerDef.h"

LookupTableNode create_node(char *lexeme, enum TOKEN_TYPE token, int line);
LookupTable create_lookup_table();
int hash(LookupTable lt, char *lexeme);
void insert(LookupTable lt, char *lexeme, enum TOKEN_TYPE token, int line);
void del(LookupTable lt, char *lexeme);
LookupTableNode search(LookupTable lt, char *lexeme);
void insert_keywords(LookupTable lt);

#endif
