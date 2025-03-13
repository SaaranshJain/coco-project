#include "parser.h"
#include "firstAndFollow.h"
#include "lexer.h"
#include "lookup.h"
#include <stdlib.h>

uint64_t *computeFirstAndFollowSets(FirstAndFollow *F, Grammar G) {
    for (int i = 0; i < NUM_NON_TERMINALS; ++i) {
        F[i] = (FirstAndFollow) malloc(sizeof(struct firstAndFollow));
        F[i]->firstSet = 0;
        F[i]->followSet = 0;
    }
    
    uint64_t *memo = computeFirstSets(F, G);
    computeFollowSets(F, G);
    return memo;
}

ParseTable createParseTable(FirstAndFollow *F, Grammar G, uint64_t *memo) {
    ParseTable T = (ParseTable) malloc(NUM_TERMINALS * NUM_NON_TERMINALS * sizeof(Rule));

    for (int i = 0; i < NUM_TERMINALS * NUM_NON_TERMINALS; ++i) {
        T[i] = NULL;
    }

    for (int i = 0; i < G->numRules; ++i) {
        Rule rule = G->rules[i];
        uint64_t firstOfThisRule = memo[i];

        if (firstOfThisRule % 2 == 1) {
            uint64_t followSetOfThisNT = F[rule->lhs - NUM_TERMINALS]->followSet;
            int j = 0;

            while (followSetOfThisNT > 0) {
                if (followSetOfThisNT % 2 == 1) {
                    T[(rule->lhs - NUM_TERMINALS) * NUM_TERMINALS + (j++)] = rule;
                }

                followSetOfThisNT >>= 1;
            }
        }

        firstOfThisRule >>= 1;
        int j = 1;

        while (firstOfThisRule > 0) {
            if (firstOfThisRule % 2 == 1) {
                T[(rule->lhs - NUM_TERMINALS) * NUM_TERMINALS + j] = rule;
            }

            firstOfThisRule >>= 1; j++;
        }
    }

    return T;
}

ParseTreeNode newNTNode(enum NON_TERMINAL nt) {
    ParseTreeNode ret = (ParseTreeNode)malloc(sizeof(struct parseTreeNode));
    ret->children = NULL;
    ret->lexeme = NULL;
    ret->nonTerminal = nt;
    ret->numChildren = 0;
    ret->parent = NULL;
    ret->token = -1;
    return ret;
}

ParseTreeNode newTnode(enum TOKEN_TYPE tk, char *lexeme) {
    ParseTreeNode ret = (ParseTreeNode)malloc(sizeof(struct parseTreeNode));
    ret->children = NULL;
    ret->lexeme = lexeme;
    ret->nonTerminal = -1;
    ret->numChildren = 0;
    ret->parent = NULL;
    ret->token = tk;
    return ret;
}

ParseTree parseInputSourceCode(char *testcaseFileName, ParseTable T, Grammar G) {
    // Tree creation
    ParseTree tree = (ParseTree)malloc(sizeof(struct parseTree));
    tree->root = newNTNode(G->startSymbol);
    ParseTreeNode currNode = tree->root;

    // Stack creation
    ParseTreeNode *stack = (ParseTreeNode *)malloc(32 * sizeof(ParseTreeNode));
    stack[0] = newTnode(TK_DOLLAR, "$");
    stack[1] = newNTNode(G->startSymbol);
    int top = 1, stackCap = 32;

    // Lexer initialisation
    FILE *testcasefile = fopen(testcaseFileName, "r");
    TwinBuffer buffer = getStream(testcasefile);
    LookupTable lt = create_lookup_table();
    TokenInfo token = getNextToken(buffer, lt);

    while (token != NULL) {
        if (stack[top]->nonTerminal == -1) {
            if (stack[top]->token == token->token) {
                currNode->lexeme = token->lexeme;
                // ADD I AND F LEXEMES AS WELL
                top--;
                token = getNextToken(buffer, lt);
                currNode = stack[top];
            } else {
                // ERROR
                fprintf(stderr, "Stack top is a terminal %s, but it doesn't match the token stream terminal %s\n", ENUM_NAME_FROM_VALUE[stack[top]->token], ENUM_NAME_FROM_VALUE[token->token]);
            }
        } else {
            Rule tableEntry = T[(stack[top]->nonTerminal - NUM_TERMINALS) * NUM_TERMINALS + token->token];
            
            if (tableEntry == NULL) {
                // ERROR
                // fprintf(stderr, "No rule from %d to %s\n", stack[top]->nonTerminal, ENUM_NAME_FROM_VALUE[token->token]);
                continue;
            }

            currNode->numChildren = tableEntry->rhsLength;
            currNode->children = (ParseTreeNode *)malloc(tableEntry->rhsLength * sizeof(ParseTreeNode));

            for (int i = 0; i < tableEntry->rhsLength; ++i) {
                if (tableEntry->rhs[i]->isTerminal) {
                    currNode->children[i] = newTnode(tableEntry->rhs[i]->symbol.terminal, NULL);
                } else {
                    currNode->children[i] = newNTNode(tableEntry->rhs[i]->symbol.nonTerminal);
                }
                currNode->children[i]->parent = currNode;
            }

            top--;
            if (top + 1 + tableEntry->rhsLength > stackCap) {
                stackCap *= 2;
                stack = (ParseTreeNode *)realloc(stack, stackCap * sizeof(ParseTreeNode));
            }

            for (int i = currNode->numChildren - 1; i >= 0; --i) {
                stack[++top] = currNode->children[i];
            }

            currNode = stack[top];
        }
    }

    if (top != 0) {
        fprintf(stderr, "Token stream ended before all elements from stack got popped\n");
    }

    return tree;
}

void preorder(ParseTreeNode node, FILE* outfile) {
    if (node->nonTerminal == -1) {
        fprintf(outfile, "(%s, %s)\n", ENUM_NAME_FROM_VALUE[node->token], node->lexeme);
    } else {
        fprintf(outfile, "%d\n", node->nonTerminal);
    }

    for (int i = 0; i < node->numChildren; ++i) {
        preorder(node->children[i], outfile);
    }
}

void printParseTree(ParseTree PT, char *outfile) {
    FILE* of = fopen(outfile, "w");
    preorder(PT->root, of);
}
