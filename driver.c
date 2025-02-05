#include <stdio.h>

// Lexer and parser headers
#include "lexer.h"
#include "parser.h"

int main(int argc, char *argv[]) {
    if(argc != 3){
        printf("Usage: %s <testcase file> <clean file>\n", argv[0]);
        return 1;
    }
    printf("Implementation status: Unimplemented\n");
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
                printf("Deleting Comments:\n");
                // removeComments(argv[1], argv[2]);
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
