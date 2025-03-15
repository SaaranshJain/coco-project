/*
    Group number 1
    Saaransh Jain - 2022A7PS0074P
    Aman Patel - 2022A7PS0152P
    Vishnu Hari - 2022A7TS0094P
    Parth Sudan - 2022A7PS0177P
*/
#include "parser.h"
#include "firstAndFollow.h"
#include "lexer.h"
#include "lookup.h"
#include <stdlib.h>

char *NONTERMINAL_NAME_FROM_VALUE[NUM_NON_TERMINALS] = {
    "NT_PROGRAM",
    "NT_MAINFUNCTION",
    "NT_OTHERFUNCTIONS",
    "NT_FUNCTION",
    "NT_INPUTPAR",
    "NT_OUTPUTPAR",
    "NT_PARAMETERLIST",
    "NT_DATATYPE",
    "NT_PRIMITIVEDATATYPE",
    "NT_CONSTRUCTEDDATATYPE",
    "NT_REMAININGLIST",
    "NT_STMTS",
    "NT_TYPEDEFINITIONS",
    "NT_ACTUALORREDEFINED",
    "NT_TYPEDEFINITION",
    "NT_FIELDDEFINITIONS",
    "NT_FIELDDEFINITION",
    "NT_MOREFIELDS",
    "NT_DECLARATIONS",
    "NT_DECLARATION",
    "NT_GLOBALORNOT",
    "NT_OTHERSTMTS",
    "NT_STMT",
    "NT_ASSIGNMENTSTMT",
    "NT_SINGLEORRECID",
    "NT_OPTIONSINGLECONSTRUCTED",
    "NT_ONEEXPANSION",
    "NT_MOREEXPANSIONS",
    "NT_FUNCALLSTMT",
    "NT_OUTPUTPARAMETERS",
    "NT_INPUTPARAMETERS",
    "NT_ITERATIVESTMT",
    "NT_CONDITIONALSTMT",
    "NT_ELSEPART",
    "NT_IOSTMT",
    "NT_ARITHMETICEXPRESSION",
    "NT_MORETERMS",
    "NT_TERM",
    "NT_MOREFACTORS",
    "NT_FACTOR",
    "NT_HIGHPRECEDENCEOPERATOR",
    "NT_LOWPRECEDENCEOPERATOR",
    "NT_BOOLEANEXPRESSION",
    "NT_VAR",
    "NT_LOGICALOP",
    "NT_RELATIONALOP",
    "NT_RETURNSTMT",
    "NT_OPTIONALRETURN",
    "NT_IDLIST",
    "NT_MOREIDS",
    "NT_DEFINETYPESTMT",
    "NT_A",
};

uint64_t *computeFirstAndFollowSets(FirstAndFollow *F, Grammar G) {
    for (int i = 0; i < NUM_NON_TERMINALS; ++i) {
        F[i] = (FirstAndFollow)malloc(sizeof(struct firstAndFollow));
        F[i]->firstSet = 0;
        F[i]->followSet = 0;
    }

    uint64_t *memo = computeFirstSets(F, G);
    computeFollowSets(F, G);
    return memo;
}

ParseTable createParseTable(FirstAndFollow *F, Grammar G, uint64_t *memo) {
    ParseTable T = (ParseTable)malloc(NUM_TERMINALS * NUM_NON_TERMINALS * sizeof(Rule));

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

                followSetOfThisNT >>= 1;
                j++;
            }
        }

        firstOfThisRule >>= 1;
        int j = 1;

        while (firstOfThisRule > 0) {
            if (firstOfThisRule & 1) {
                T[(rule->lhs - NUM_TERMINALS) * NUM_TERMINALS + j] = rule;
            }

            firstOfThisRule >>= 1;
            j++;
        }
    }

    return T;
}

ParseTreeNode newNTNode(enum NON_TERMINAL nt, int line) {
    ParseTreeNode ret = (ParseTreeNode)malloc(sizeof(struct parseTreeNode));
    ret->isTerminal = false;
    ret->token = TK_DOLLAR;
    ret->nonTerminal = nt;
    ret->lexeme = NULL;
    ret->lexemeI = 0;
    ret->lexemeF = 0.0;
    ret->line = line;

    ret->children = NULL;
    ret->numChildren = 0;
    ret->parent = NULL;
    return ret;
}

ParseTreeNode newTnode(enum TOKEN_TYPE tk, char *lexeme, int lexemeI, double lexemeF, int line) {
    ParseTreeNode ret = (ParseTreeNode)malloc(sizeof(struct parseTreeNode));
    ret->isTerminal = true;
    ret->token = tk;
    ret->nonTerminal = NT_PROGRAM;
    ret->lexeme = lexeme;
    ret->lexemeI = lexemeI;
    ret->lexemeF = lexemeF;
    ret->line = line;

    ret->children = NULL;
    ret->numChildren = 0;
    ret->parent = NULL;
    return ret;
}

ParseTree parseInputSourceCode(char *testcaseFileName, ParseTable T, Grammar G) {
    // Tree creation
    ParseTree tree = (ParseTree)malloc(sizeof(struct parseTree));
    tree->root = newNTNode(G->startSymbol, 1);
    ParseTreeNode currNode = tree->root;

    // Stack creation
    ParseTreeNode *stack = (ParseTreeNode *)malloc(32 * sizeof(ParseTreeNode));
    stack[0] = newTnode(TK_DOLLAR, "$", 0, 0.0, 1);
    stack[1] = currNode;
    int top = 1, stackCap = 32;

    // Lexer initialisation
    FILE *testcasefile = fopen(testcaseFileName, "r");
    TwinBuffer buffer = getStream(testcasefile);
    LookupTable lt = create_lookup_table();
    TokenInfo token = getNextToken(buffer, lt);

    while (token != NULL && top >= 0) {
        if (stack[top]->isTerminal) {
            if (stack[top]->token == token->token) {
                currNode->lexeme = token->lexeme;
                currNode->lexemeI = token->lexemeI;
                currNode->lexemeF = token->lexemeF;
                currNode->line = buffer->line;
                top--;
                token = getNextToken(buffer, lt);
                currNode = stack[top];
            } else {
                if (token->token == ERROR) {
                    if ((token = getNextToken(buffer, lt)) == NULL)
                        break;
                }

                fprintf(stderr, "Line %d\tError: The token %s for lexeme %s does not match with the expected token %s\n", buffer->line, TOKEN_NAME_FROM_VALUE[token->token], token->lexeme, TOKEN_NAME_FROM_VALUE[stack[top]->token]);
                int prevLine = buffer->line;
                while (token != NULL && buffer->line == prevLine) {
                    token = getNextToken(buffer, lt);
                }

                while (top >= 0 && ((stack[top]->isTerminal && stack[top]->token != token->token) || (!stack[top]->isTerminal && T[(stack[top]->nonTerminal - NUM_TERMINALS) * NUM_TERMINALS + token->token] == NULL))) {
                    top--;
                }
            }
        } else {
            Rule tableEntry = T[(stack[top]->nonTerminal - NUM_TERMINALS) * NUM_TERMINALS + token->token];

            if (tableEntry == NULL) {
                if (token->token == ERROR) {
                    if ((token = getNextToken(buffer, lt)) == NULL)
                        break;
                }

                if (T[(stack[top]->nonTerminal - NUM_TERMINALS) * NUM_TERMINALS + token->token] == NULL) {
                    fprintf(stderr, "Line %d\tError: Invalid token %s encountered with value %s stack top %s\n", buffer->line, TOKEN_NAME_FROM_VALUE[token->token], token->lexeme, NONTERMINAL_NAME_FROM_VALUE[stack[top]->nonTerminal - NUM_TERMINALS]);
                }

                int prevLine = buffer->line;
                while (token != NULL && buffer->line == prevLine) {
                    token = getNextToken(buffer, lt);
                }

                while (top >= 0 && ((stack[top]->isTerminal && stack[top]->token != token->token) || (!stack[top]->isTerminal && T[(stack[top]->nonTerminal - NUM_TERMINALS) * NUM_TERMINALS + token->token] == NULL))) {
                    top--;
                }

                if (top < 0) {
                    break;
                }

                continue;
            }

            if (tableEntry->isEpsilon) {
                top--;
                currNode->line = buffer->line;
                currNode->numChildren = 1;
                currNode->children = (ParseTreeNode *)malloc(sizeof(ParseTreeNode));
                currNode->children[0] = newTnode(TK_DOLLAR, "eps", 0, 0.0, buffer->line);
                currNode->children[0]->parent = currNode;
                currNode = stack[top];
                continue;
            }

            currNode->line = buffer->line;
            currNode->numChildren = tableEntry->rhsLength;
            currNode->children = (ParseTreeNode *)malloc(tableEntry->rhsLength * sizeof(ParseTreeNode));

            for (int i = 0; i < tableEntry->rhsLength; ++i) {
                if (tableEntry->rhs[i]->isTerminal) {
                    currNode->children[i] = newTnode(tableEntry->rhs[i]->symbol.terminal, NULL, 0, 0.0, buffer->line);
                } else {
                    currNode->children[i] = newNTNode(tableEntry->rhs[i]->symbol.nonTerminal, buffer->line);
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
        fprintf(stderr, "Unexpected end of file\n");
    }

    return tree;
}

void inorder(ParseTreeNode node, FILE *outfile) {
    if (node->numChildren > 0)
        inorder(node->children[0], outfile);

    if (node->isTerminal) {
        if (node->token == TK_DOLLAR) {
            fprintf(outfile, "ε\tε\t%d\tε\tN/A\t%s\tyes\tε\n", node->line, NONTERMINAL_NAME_FROM_VALUE[node->parent->nonTerminal - NUM_TERMINALS]);
        } else if (node->token == TK_NUM) {
            fprintf(outfile, "%s\t%s\t%d\tTK_NUM\t%d\t%s\tyes\t%s\n", node->lexeme, node->lexeme, node->line, node->lexemeI, NONTERMINAL_NAME_FROM_VALUE[node->parent->nonTerminal - NUM_TERMINALS], node->lexeme);
        } else if (node->token == TK_RNUM) {
            fprintf(outfile, "%s\t%s\t%d\tTK_RNUM\t%.4lf\t%s\tyes\t%s\n", node->lexeme, node->lexeme, node->line, node->lexemeF, NONTERMINAL_NAME_FROM_VALUE[node->parent->nonTerminal - NUM_TERMINALS], node->lexeme);
        } else {
            fprintf(outfile, "%s\t%s\t%d\t%s\tN/A\t%s\tyes\t%s\n", node->lexeme, node->lexeme, node->line, TOKEN_NAME_FROM_VALUE[node->token], NONTERMINAL_NAME_FROM_VALUE[node->parent->nonTerminal - NUM_TERMINALS], node->lexeme);
        }
    } else {
        if (node->parent != NULL) {
            fprintf(outfile, "----\t%s\t%d\tN/A\tN/A\t%s\tno\t%s\n", NONTERMINAL_NAME_FROM_VALUE[node->nonTerminal - NUM_TERMINALS], node->line, NONTERMINAL_NAME_FROM_VALUE[node->parent->nonTerminal - NUM_TERMINALS], NONTERMINAL_NAME_FROM_VALUE[node->nonTerminal - NUM_TERMINALS]);
        } else {
            fprintf(outfile, "----\t%s\t%d\tN/A\tN/A\tROOT\tno\t%s\n", NONTERMINAL_NAME_FROM_VALUE[node->nonTerminal - NUM_TERMINALS], node->line, NONTERMINAL_NAME_FROM_VALUE[node->nonTerminal - NUM_TERMINALS]);
        }
    }

    for (int i = 1; i < node->numChildren; ++i) {
        inorder(node->children[i], outfile);
    }
}

void printParseTree(ParseTree PT, char *outfile) {
    FILE *of = fopen(outfile, "w");
    inorder(PT->root, of);
    fclose(of);
}
