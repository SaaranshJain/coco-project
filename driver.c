/*
    Group number 1
    Saaransh Jain - 2022A7PS0074P
    Aman Patel - 2022A7PS0152P
    Vishnu Hari - 2022A7TS0094P
    Parth Sudan - 2022A7PS0177P
*/
#include "grammar.h"
#include "lexer.h"
#include "lookup.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char *argv[]) {
    // printf("Implementation status: Comment cleaning implemented\n");
    // printf("Implementation status: Testing tokens\n");
    // printf("Implementation status: Testing parsing\n");
    printf("Implementation status: Both lexical and syntax analysis modules implemented\n");
    printf("------------------------------------\n");

    // Automatic computing of first set, follow set, parse table at the start of the program
    FirstAndFollow *F = (FirstAndFollow *)malloc(NUM_NON_TERMINALS * sizeof(FirstAndFollow));
    uint64_t *memo = computeFirstAndFollowSets(F, &languageGrammar);
    ParseTable T = createParseTable(F, &languageGrammar, memo);
    ParseTree tree;

    while (1) {
        printf("Choose an option:\n");
        printf("0. Exit\n");
        printf("1. Remove Comments\n");
        printf("2. Print Tokens\n");
        printf("3. Generate Parse Tree\n");
        printf("4. Measure time taken\n");

        int input;
        if (scanf("%d", &input) != 1) {
            printf("Invalid input\n");
            return 1;
        }

        switch (input) {
        case 0:
            return 0;
        case 1:
            if (argc != 3) {
                printf("Usage: %s <testcase file> <clean file>\n", argv[0]);
                return 1;
            }

            printf("Code after deleting comments:\n");
            printf("-----------------------------\n");
            removeComments(argv[1], argv[2]);

            FILE *cleanFile = fopen(argv[2], "r");

            if (cleanFile == NULL) {
                printf("Error opening clean file\n");
                return 1;
            }

            char line[2048];

            while (fgets(line, sizeof(line), cleanFile)) {
                printf("%s", line);
            }

            fclose(cleanFile);
            printf("--------------------------------\n");
            printf("Clean file generated\n");
            break;
        case 2:
            if (argc < 2) {
                printf("Usage: %s <testcase file>\n", argv[0]);
                return 1;
            }

            printf("Printing Tokens:\n");
            FILE *testcasefile = fopen(argv[1], "r");
            TwinBuffer buffer = getStream(testcasefile);
            TokenInfo token;
            LookupTable lt = create_lookup_table();

            while ((token = getNextToken(buffer, lt)) != NULL) {
                if (token->token == ERROR) continue;
                printf("Line no. %d\t", buffer->line);

                if (token->token == TK_NUM) {
                    printf("Lexeme %d", token->lexemeI);
                } else if (token->token == TK_RNUM) {
                    printf("Lexeme %f", token->lexemeF);
                } else {
                    printf("Lexeme %.*s", token->lexemeLength, token->lexeme);
                }

                printf("\tToken %s\n", TOKEN_NAME_FROM_VALUE[token->token]);
            }

            break;
        case 3:
            if (argc < 3) {
                printf("Usage: %s <testcase file> <output file>\n", argv[0]);
                return 1;
            }

            printf("Generating parse tree\n");
            tree = parseInputSourceCode(argv[1], T, &languageGrammar);
            printParseTree(tree, argv[2]);
            break;
        case 4:
            if (argc < 2) {
                printf("Usage: %s <testcase file>\n", argv[0]);
                return 1;
            }
            
            clock_t start_time, end_time;
            double total_CPU_time, total_CPU_time_in_seconds;
            start_time = clock();
            tree = parseInputSourceCode(argv[1], T, &languageGrammar);
            end_time = clock();
            total_CPU_time = (double) (end_time - start_time);
            total_CPU_time_in_seconds = total_CPU_time / CLOCKS_PER_SEC;
            printf("Time taken for parser (which automatically calls lexer to generate tokens) is %lf seconds\n", total_CPU_time_in_seconds);
            break;
        default:
            printf("Invalid input\n");
            break;
        }
        printf("------------------------------------\n");
    }
}
