#include "lookup.h"
#include <stdlib.h>
#include <string.h>

LookupTableNode create_node(char *lexeme, enum TOKEN_TYPE token, int line)
{
    LookupTableNode newnode = (LookupTableNode)malloc(sizeof(struct lookupTableNode));
    newnode->lexeme = lexeme;
    newnode->defined_at_line = line;
    newnode->token = token;
    newnode->next = NULL;
    return newnode;
}

LookupTable create_lookup_table()
{
    LookupTable lt = (LookupTable)malloc(sizeof(struct lookupTable));
    lt->size = 0;
    lt->capacity = 100;
    lt->nodes = (LookupTableNode *)malloc(100 * sizeof(LookupTableNode));
    insert_keywords(lt);
    return lt;
}

int hash(LookupTable lt, char *lexeme)
{
    int bucketIndex;
    int sum = 0, factor = 31;
    for (int i = 0; i < strlen(lexeme); i++)
    {
        sum = ((sum % lt->capacity) + (((int)lexeme[i]) * factor) % lt->capacity) % lt->capacity;
        factor = ((factor % 0x7fff) * (31 % 0x7fff)) % 0x7fff;
    }

    bucketIndex = sum;
    return bucketIndex;
}

void insert(LookupTable lt, char *lexeme, enum TOKEN_TYPE token, int line)
{
    int bucketIndex = hash(lt, lexeme);
    LookupTableNode newnode = create_node(lexeme, token, line);

    if (lt->nodes[bucketIndex] == NULL)
    {
        lt->nodes[bucketIndex] = newnode;
    }
    else
    {
        newnode->next = lt->nodes[bucketIndex];
        lt->nodes[bucketIndex] = newnode;
    }
}

void del(LookupTable lt, char *lexeme)
{
    int bucketIndex = hash(lt, lexeme);
    LookupTableNode prevNode = NULL;
    LookupTableNode currNode = lt->nodes[bucketIndex];
 
    while (currNode != NULL) {
        if (strcmp(lexeme, currNode->lexeme) == 0) {
            if (currNode == lt->nodes[bucketIndex]) {
                lt->nodes[bucketIndex] = currNode->next;
            }
            else {
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

LookupTableNode search(LookupTable lt, char *lexeme)
{
    int bucketIndex = hash(lt, lexeme);
    LookupTableNode currNode = lt->nodes[bucketIndex];

    while (currNode != NULL)
    {
        if (strcmp(lexeme, currNode->lexeme) == 0)
        {
            return currNode;
        }

        currNode = currNode->next;
    }

    return NULL;
}

void insert_keywords(LookupTable lt) {
    insert(lt, "with", TK_WITH, 0);
    insert(lt, "parameters", TK_PARAMETERS, 0);
    insert(lt, "end", TK_END, 0);
    insert(lt, "while", TK_WHILE, 0);
    insert(lt, "union", TK_UNION, 0);
    insert(lt, "endunion", TK_ENDUNION, 0);
    insert(lt, "definetype", TK_DEFINETYPE, 0);
    insert(lt, "as", TK_AS, 0);
    insert(lt, "type", TK_TYPE, 0);
    insert(lt, "_main", TK_MAIN, 0);
    insert(lt, "global", TK_GLOBAL, 0);
    insert(lt, "parameter", TK_PARAMETER, 0);
    insert(lt, "list", TK_LIST, 0);
    insert(lt, "input", TK_INPUT, 0);
    insert(lt, "output", TK_OUTPUT, 0);
    insert(lt, "int", TK_INT, 0);
    insert(lt, "real", TK_REAL, 0);
    insert(lt, "endwhile", TK_ENDWHILE, 0);
    insert(lt, "if", TK_IF, 0);
    insert(lt, "then", TK_THEN, 0);
    insert(lt, "endif", TK_ENDIF, 0);
    insert(lt, "read", TK_READ, 0);
    insert(lt, "write", TK_WRITE, 0);
    insert(lt, "return", TK_RETURN, 0);
    insert(lt, "call", TK_CALL, 0);
    insert(lt, "record", TK_RECORD, 0);
    insert(lt, "endrecord", TK_ENDRECORD, 0);
    insert(lt, "else", TK_ELSE, 0);
}
