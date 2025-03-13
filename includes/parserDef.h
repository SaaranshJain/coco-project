#ifndef PARSERDEF_H
#define PARSERDEF_H

#include "lexerDef.h"
#define NUM_NON_TERMINALS 52

enum NON_TERMINAL
{
    NT_Program = NUM_TERMINALS,
    NT_MainFunction,
    NT_OtherFunctions,
    NT_Function,
    NT_InputPar,
    NT_OutputPar,
    NT_ParameterList,
    NT_DataType,
    NT_PrimitiveDatatype,
    NT_ConstructedDatatype,
    NT_RemainingList,
    NT_Stmts,
    NT_TypeDefinitions,
    NT_ActualOrRedefined,
    NT_TypeDefinition,
    NT_FieldDefinitions,
    NT_FieldDefinition,
    NT_MoreFields,
    NT_Declarations,
    NT_Declaration,
    NT_GlobalOrNot,
    NT_OtherStmts,
    NT_Stmt,
    NT_AssignmentStmt,
    NT_SingleOrRecId,
    NT_OptionSingleConstructed,
    NT_OneExpansion,
    NT_MoreExpansions,
    NT_FunCallStmt,
    NT_OutputParameters,
    NT_InputParameters,
    NT_IterativeStmt,
    NT_ConditionalStmt,
    NT_ElsePart,
    NT_IoStmt,
    NT_ArithmeticExpression,
    NT_MoreTerms,
    NT_Term,
    NT_MoreFactors,
    NT_Factor,
    NT_HighPrecedenceOperator,
    NT_LowPrecedenceOperator,
    NT_BooleanExpression,
    NT_Var,
    NT_LogicalOp,
    NT_RelationalOp,
    NT_ReturnStmt,
    NT_OptionalReturn,
    NT_IdList,
    NT_MoreIds,
    NT_DefinetypeStmt,
    NT_A,
};

struct ruleRightSideElement
{
    bool isEpsilon;
    bool isTerminal;
    union symbol
    {
        enum TOKEN_TYPE terminal;
        enum NON_TERMINAL nonTerminal;
    } symbol;
};

typedef struct ruleRightSideElement* RuleRightSideElement;

struct rule
{
    enum NON_TERMINAL lhs;
    int rhsLength;
    RuleRightSideElement* rhs;
};

typedef struct rule* Rule;

struct grammar
{
    enum NON_TERMINAL startSymbol;
    int numRules;
    Rule* rules;
};

typedef struct grammar* Grammar;

struct firstAndFollow
{
    enum NON_TERMINAL nonTerminal;
    int firstSetSize;
    int firstSetCapacity;
    enum TOKEN_TYPE* firstSet;
    bool firstSetEpsilon;
    int followSetSize;
    int followSetCapacity;
    enum TOKEN_TYPE* followSet;
    bool followSetDollar;
};

typedef struct firstAndFollow* FirstAndFollow;
typedef Rule* ParseTable;

struct parseTreeNode
{
    enum TOKEN_TYPE token;
    enum NON_TERMINAL nonTerminal;
    char* lexeme;
    struct parseTreeNode* parent;
    struct parseTreeNode** children;
    int numChildren;
};

struct parseTree
{
    struct parseTreeNode* root;
};

typedef struct parseTree* ParseTree;

#endif
