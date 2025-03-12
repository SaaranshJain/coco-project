#include "lexer.h"
#include "lookup.h"
#include <stdio.h>
#include <sys/types.h>
// #include "parser.h"

int main(int argc, char *argv[]) {
    // printf("Implementation status: Comment cleaning implemented\n");
    // printf("Implementation status: Testing tokens\n");
    printf("Implementation status: Testing parsing\n");
    printf("------------------------------------\n");

    while (1) {
        printf("Choose an option:\n");
        printf("0. Exit\n");
        printf("1. Remove Comments\n");
        printf("2. Print Tokens\n");
        printf("3. Generate Parse Tree\n");

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
                printf("Token type: %s\n", ENUM_NAME_FROM_VALUE[token->token]);

                if (token->token == TK_NUM) {
                    printf("Lexeme: %d\n", token->lexemeI);
                } else if (token->token == TK_RNUM) {
                    printf("Lexeme: %f\n", token->lexemeF);
                } else {
                    printf("Lexeme: %.*s\n", token->lexemeLength, token->lexeme);
                }

                printf("\n");
            }

            break;
        case 3:
            printf("Generating parse tree\n");
            break;
        default:
            printf("Invalid input\n");
            break;
        }
        printf("------------------------------------\n");
    }
}
