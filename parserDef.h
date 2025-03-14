#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "lexerDef.h"
#include <stdint.h>
#define NUM_NON_TERMINALS 52

enum NON_TERMINAL {
    NT_PROGRAM = NUM_TERMINALS,
    NT_MAINFUNCTION,
    NT_OTHERFUNCTIONS,
    NT_FUNCTION,
    NT_INPUTPAR,
    NT_OUTPUTPAR,
    NT_PARAMETERLIST,
    NT_DATATYPE,
    NT_PRIMITIVEDATATYPE,
    NT_CONSTRUCTEDDATATYPE,
    NT_REMAININGLIST,
    NT_STMTS,
    NT_TYPEDEFINITIONS,
    NT_ACTUALORREDEFINED,
    NT_TYPEDEFINITION,
    NT_FIELDDEFINITIONS,
    NT_FIELDDEFINITION,
    NT_MOREFIELDS,
    NT_DECLARATIONS,
    NT_DECLARATION,
    NT_GLOBALORNOT,
    NT_OTHERSTMTS,
    NT_STMT,
    NT_ASSIGNMENTSTMT,
    NT_SINGLEORRECID,
    NT_OPTIONSINGLECONSTRUCTED,
    NT_ONEEXPANSION,
    NT_MOREEXPANSIONS,
    NT_FUNCALLSTMT,
    NT_OUTPUTPARAMETERS,
    NT_INPUTPARAMETERS,
    NT_ITERATIVESTMT,
    NT_CONDITIONALSTMT,
    NT_ELSEPART,
    NT_IOSTMT,
    NT_ARITHMETICEXPRESSION,
    NT_MORETERMS,
    NT_TERM,
    NT_MOREFACTORS,
    NT_FACTOR,
    NT_HIGHPRECEDENCEOPERATOR,
    NT_LOWPRECEDENCEOPERATOR,
    NT_BOOLEANEXPRESSION,
    NT_VAR,
    NT_LOGICALOP,
    NT_RELATIONALOP,
    NT_RETURNSTMT,
    NT_OPTIONALRETURN,
    NT_IDLIST,
    NT_MOREIDS,
    NT_DEFINETYPESTMT,
    NT_A,
};

struct ruleRightSideElement {
    bool isTerminal;
    union symbol {
        enum TOKEN_TYPE terminal;
        enum NON_TERMINAL nonTerminal;
    } symbol;
};

typedef struct ruleRightSideElement *RuleRightSideElement;

struct rule {
    enum NON_TERMINAL lhs;
    int rhsLength;
    RuleRightSideElement *rhs;
    bool isEpsilon;
};

typedef struct rule *Rule;

struct grammar {
    enum NON_TERMINAL startSymbol;
    int numRules;
    Rule *rules;
};

typedef struct grammar *Grammar;

struct firstAndFollow {
    // enum NON_TERMINAL nonTerminal;
    // int firstSetSize;
    uint64_t firstSet;
    // bool firstSetEpsilon;
    // int followSetSize;
    uint64_t followSet;
    // bool followSetDollar;
};

typedef struct firstAndFollow *FirstAndFollow;
typedef Rule *ParseTable;

struct parseTreeNode {
    bool isTerminal;
    enum TOKEN_TYPE token;
    enum NON_TERMINAL nonTerminal;
    char *lexeme;
    int lexemeI;
    double lexemeF;
    struct parseTreeNode *parent;
    struct parseTreeNode **children;
    int numChildren;
};

typedef struct parseTreeNode *ParseTreeNode;

struct parseTree {
    ParseTreeNode root;
};

typedef struct parseTree *ParseTree;

#endif
