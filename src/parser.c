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

        if (firstOfThisRule & 1) {
            uint64_t followSetOfThisNT = F[rule->lhs - NUM_TERMINALS]->followSet;
            int j = 0;

            while (followSetOfThisNT > 0) {
                if (followSetOfThisNT & 1) {
                    T[(rule->lhs - NUM_TERMINALS) * NUM_TERMINALS + j] = rule;
                }

                followSetOfThisNT >>= 1; j++;
            }
        }

        firstOfThisRule >>= 1;
        int j = 1;

        while (firstOfThisRule > 0) {
            if (firstOfThisRule & 1) {
                T[(rule->lhs - NUM_TERMINALS) * NUM_TERMINALS + j] = rule;
            }

            firstOfThisRule >>= 1; j++;
        }
    }

    return T;
}

ParseTreeNode newNTNode(enum NON_TERMINAL nt) {
    ParseTreeNode ret = (ParseTreeNode)malloc(sizeof(struct parseTreeNode));
    ret->isTerminal = false;
    ret->token = TK_DOLLAR;
    ret->nonTerminal = nt;
    ret->lexeme = NULL;
    ret->lexemeI = 0;
    ret->lexemeF = 0.0;

    ret->children = NULL;
    ret->numChildren = 0;
    ret->parent = NULL;
    return ret;
}

ParseTreeNode newTnode(enum TOKEN_TYPE tk, char *lexeme, int lexemeI, double lexemeF) {
    ParseTreeNode ret = (ParseTreeNode)malloc(sizeof(struct parseTreeNode));
    ret->isTerminal = true;
    ret->token = tk;
    ret->nonTerminal = NT_PROGRAM;
    ret->lexeme = lexeme;
    ret->lexemeI = lexemeI;
    ret->lexemeF = lexemeF;

    ret->children = NULL;
    ret->numChildren = 0;
    ret->parent = NULL;
    return ret;
}

ParseTree parseInputSourceCode(char *testcaseFileName, ParseTable T, Grammar G) {
    // Tree creation
    ParseTree tree = (ParseTree)malloc(sizeof(struct parseTree));
    tree->root = newNTNode(G->startSymbol);
    ParseTreeNode currNode = tree->root;

    // Stack creation
    ParseTreeNode *stack = (ParseTreeNode *)malloc(32 * sizeof(ParseTreeNode));
    stack[0] = newTnode(TK_DOLLAR, "$", 0, 0.0);
    stack[1] = newNTNode(G->startSymbol);
    int top = 1, stackCap = 32;

    // Lexer initialisation
    FILE *testcasefile = fopen(testcaseFileName, "r");
    TwinBuffer buffer = getStream(testcasefile);
    LookupTable lt = create_lookup_table();
    TokenInfo token = getNextToken(buffer, lt);

    while (token != NULL) {
        if (stack[top]->isTerminal) {
            if (stack[top]->token == token->token) {
                currNode->lexeme = token->lexeme;
                currNode->lexemeI = token->lexemeI;
                currNode->lexemeF = token->lexemeF;
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
                fprintf(stderr, "No rule from %d to %s\n", stack[top]->nonTerminal, ENUM_NAME_FROM_VALUE[token->token]);
                break;
            }

            if (tableEntry->isEpsilon) {
                top--;
                continue;
            }

            currNode->numChildren = tableEntry->rhsLength;
            currNode->children = (ParseTreeNode *)malloc(tableEntry->rhsLength * sizeof(ParseTreeNode));

            for (int i = 0; i < tableEntry->rhsLength; ++i) {
                if (tableEntry->rhs[i]->isTerminal) {
                    currNode->children[i] = newTnode(tableEntry->rhs[i]->symbol.terminal, NULL, 0, 0.0);
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
        fprintf(stderr, "Token stream ended before all elements from stack got popped. Top = %d\n", top);
    }

    return tree;
}

void preorder(ParseTreeNode node, FILE* outfile) {
    if (node->isTerminal) {
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
    fclose(of);
}
