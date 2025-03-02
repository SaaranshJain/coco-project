#ifndef LEXERDEF_H
#define LEXERDEF_H

#include <stdbool.h>
#include <stdio.h>
#define TWIN_BUFFER_INDIVIDUAL_BUFFER_SIZE 2048

enum TOKEN_TYPE
{
    TK_ASSIGNOP, TK_COMMENT, TK_FIELDID, TK_ID,
    TK_NUM, TK_RNUM, TK_FUNID, TK_RUID,
    TK_WITH, TK_PARAMETERS, TK_END, TK_WHILE,
    TK_UNION, TK_ENDUNION, TK_DEFINETYPE, TK_AS,
    TK_TYPE, TK_MAIN, TK_GLOBAL, TK_PARAMETER,
    TK_LIST, TK_SQL, TK_SQR, TK_INPUT,
    TK_OUTPUT, TK_INT, TK_REAL, TK_COMMA,
    TK_SEM, TK_COLON, TK_DOT, TK_ENDWHILE,
    TK_OP, TK_CL, TK_IF, TK_THEN,
    TK_ENDIF, TK_READ, TK_WRITE, TK_RETURN,
    TK_PLUS, TK_MINUS, TK_MUL, TK_DIV,
    TK_CALL, TK_RECORD, TK_ENDRECORD, TK_ELSE,
    TK_AND, TK_OR, TK_NOT, TK_LT,
    TK_LE, TK_EQ, TK_GT, TK_GE,
    TK_NE, ERROR
};

enum STATE
{
    START, FIRST_AND, FIRST_OR, FIRST_EQ, FIRST_NOT, FIRST_LT, FIRST_GT, UNDERSCORE,
    HASHTAG, PERCENT, FIRST_ID, FIRST_FIELDID, FIRST_DIGIT, SECOND_AND, SECOND_OR, LT_MINUS,
    UNDERSCORE_LETTER, HASHTAG_LETTER, FIELD_ID, SECOND_ID, DIGIT_DOT, ASSIGNOP,
    FUNID, NUMBER_ID, FIRST_FLDIG, SECOND_FLDIG, FL_EXP, FL_EXP_SIGNED, FL_EXP_COMPLETE
};

extern char* ENUM_NAME_FROM_VALUE[58];

struct tokenInfo
{
    enum TOKEN_TYPE token;
    char *lexeme;
    int lexemeLength;
    int lexemeI;
    double lexemeF;
};

typedef struct tokenInfo *TokenInfo;

struct twinBuffer
{
    char buffer1[TWIN_BUFFER_INDIVIDUAL_BUFFER_SIZE];
    char buffer2[TWIN_BUFFER_INDIVIDUAL_BUFFER_SIZE];
    char *currentBuffer;
    int currentPos;
    int bufferSize;
    FILE *fp;
    int line;
};

typedef struct twinBuffer *TwinBuffer;

struct lookupTableNode
{
    char *lexeme;
    enum TOKEN_TYPE token;
    int defined_at_line;
    struct lookupTableNode *next;
};

typedef struct lookupTableNode *LookupTableNode;

struct lookupTable
{
    int size, capacity;
    LookupTableNode* nodes;
};

typedef struct lookupTable *LookupTable;

#endif
