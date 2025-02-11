#include <stdio.h>
#include <sys/types.h>
#include "lexer.h"
// #include "parser.h"

int main(int argc, char *argv[]) {
    // printf("Implementation status: Comment cleaning implemented\n");
    printf("Implementation status: Testing tokens\n");
    printf("------------------------------------\n");

    while (1) {
        printf("Choose an option:\n");
        printf("0. Exit\n");
        printf("1. Remove Comments\n");
        printf("2. Print Tokens\n");
        printf("3. Generate Parse Tree\n");

        int input;
        scanf("%d", &input);

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
                
                char *line = NULL;
                size_t len = 0;
                ssize_t read;

                while ((read = getline(&line, &len, cleanFile)) != -1) {
                    printf("%s", line);
                }

                fclose(cleanFile);
                printf("--------------------------------\n");
                printf("Clean file generated\n");
                break;
            case 2:
                if (argc != 2) {
                    printf("Usage: %s <testcase file>\n", argv[0]);
                    return 1;
                }

                printf("Printing Tokens:\n");
                FILE* testcasefile = fopen(argv[1], "r");
                TwinBuffer buffer = getStream(testcasefile);
                TokenInfo token;

                while((token = getNextToken(buffer)) != NULL) {
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
