#include <stdio.h>

// Lexer and parser headers
#include "lexer.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    if(argc != 3){
        printf("Usage: %s <testcase file> <clean file>\n", argv[0]);
        return 1;
    }
    printf("Implementation status: Comment cleaning implemented\n");
    printf("------------------------------------\n");
    while(1){
        printf("Choose an option:\n");
        printf("0. Exit\n");
        printf("1. Remove Comments\n");
        printf("2. Print Tokens\n");
        printf("3. Generate Parse Tree\n");
        int input;
        scanf("%d", &input);
        switch(input){
            case 0:
                return 0;
            case 1:
                printf("Code after deleting comments:\n");
                printf("-----------------------------\n");
                removeComments(argv[1], argv[2]);
                // Efficiently read the clean file and print it
                FILE *cleanFile = fopen(argv[2], "r");
                if(cleanFile == NULL){
                    printf("Error opening clean file\n");
                    return 1;
                }
                // Read the clean file line by line
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
                printf("Printing Tokens:\n");
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
