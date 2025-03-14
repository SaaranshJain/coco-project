/*
    Group number 1
    Saaransh Jain - 2022A7PS0074P
    Aman Patel - 2022A7PS0152P
    Vishnu Hari - 2022A7TS0094P
    Parth Sudan - 2022A7PS0177P
*/
#include "lookup.h"
#include <stdlib.h>
#include <string.h>

LookupTableNode create_node(char *lexeme, enum TOKEN_TYPE token, int line) {
    LookupTableNode newnode = (LookupTableNode)malloc(sizeof(struct lookupTableNode));
    newnode->lexeme = lexeme;
    newnode->defined_at_line = line;
    newnode->token = token;
    newnode->next = NULL;
    return newnode;
}

LookupTable create_lookup_table() {
    LookupTable lt = (LookupTable)malloc(sizeof(struct lookupTable));
    lt->size = 0;
    lt->capacity = 100;
    lt->nodes = (LookupTableNode *)malloc(100 * sizeof(LookupTableNode));
    insert_keywords(lt);
    return lt;
}

int hash(LookupTable lt, char *lexeme, int lexemeLength) {
    int bucketIndex;
    int sum = 0, factor = 31;
    for (int i = 0; i < lexemeLength; i++) {
        sum = ((sum % lt->capacity) + (((int)lexeme[i]) * factor) % lt->capacity) % lt->capacity;
        factor = ((factor % 0x7fff) * (31 % 0x7fff)) % 0x7fff;
    }

    bucketIndex = sum;
    return bucketIndex;
}

void insert(LookupTable lt, char *lexeme, int lexemeLength, enum TOKEN_TYPE token, int line) {
    int bucketIndex = hash(lt, lexeme, lexemeLength);
    LookupTableNode newnode = create_node(lexeme, token, line);

    if (lt->nodes[bucketIndex] == NULL) {
        lt->nodes[bucketIndex] = newnode;
    } else {
        newnode->next = lt->nodes[bucketIndex];
        lt->nodes[bucketIndex] = newnode;
    }
}

void del(LookupTable lt, char *lexeme, int lexemeLength) {
    int bucketIndex = hash(lt, lexeme, lexemeLength);
    LookupTableNode prevNode = NULL;
    LookupTableNode currNode = lt->nodes[bucketIndex];

    while (currNode != NULL) {
        if (strcmp(lexeme, currNode->lexeme) == 0) {
            if (currNode == lt->nodes[bucketIndex]) {
                lt->nodes[bucketIndex] = currNode->next;
            } else {
                prevNode->next = currNode->next;
            }

            free(currNode);
            break;
        }

        prevNode = currNode;
        currNode = currNode->next;
    }

    return;
}

LookupTableNode search(LookupTable lt, char *lexeme, int lexemeLength) {
    int bucketIndex = hash(lt, lexeme, lexemeLength);
    LookupTableNode currNode = lt->nodes[bucketIndex];

    while (currNode != NULL) {
        if (strcmp(lexeme, currNode->lexeme) == 0) {
            return currNode;
        }

        currNode = currNode->next;
    }

    return NULL;
}

void insert_keywords(LookupTable lt) {
    insert(lt, "with", 4, TK_WITH, 0);
    insert(lt, "parameters", 10, TK_PARAMETERS, 0);
    insert(lt, "end", 3, TK_END, 0);
    insert(lt, "while", 5, TK_WHILE, 0);
    insert(lt, "union", 5, TK_UNION, 0);
    insert(lt, "endunion", 8, TK_ENDUNION, 0);
    insert(lt, "definetype", 10, TK_DEFINETYPE, 0);
    insert(lt, "as", 2, TK_AS, 0);
    insert(lt, "type", 4, TK_TYPE, 0);
    insert(lt, "_main", 5, TK_MAIN, 0);
    insert(lt, "global", 6, TK_GLOBAL, 0);
    insert(lt, "parameter", 9, TK_PARAMETER, 0);
    insert(lt, "list", 4, TK_LIST, 0);
    insert(lt, "input", 5, TK_INPUT, 0);
    insert(lt, "output", 6, TK_OUTPUT, 0);
    insert(lt, "int", 3, TK_INT, 0);
    insert(lt, "real", 4, TK_REAL, 0);
    insert(lt, "endwhile", 8, TK_ENDWHILE, 0);
    insert(lt, "if", 2, TK_IF, 0);
    insert(lt, "then", 4, TK_THEN, 0);
    insert(lt, "endif", 5, TK_ENDIF, 0);
    insert(lt, "read", 4, TK_READ, 0);
    insert(lt, "write", 5, TK_WRITE, 0);
    insert(lt, "return", 6, TK_RETURN, 0);
    insert(lt, "call", 4, TK_CALL, 0);
    insert(lt, "record", 6, TK_RECORD, 0);
    insert(lt, "endrecord", 9, TK_ENDRECORD, 0);
    insert(lt, "else", 4, TK_ELSE, 0);
}
