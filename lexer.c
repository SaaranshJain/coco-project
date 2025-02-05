#include <stdio.h>
#include <sys/types.h>
#include "lexer.h"
#include "lexerDef.h"

FILE *getStream(FILE *fp) {
    // to be implemented
}

tokenInfo getNextToken(twinBuffer B) {
    
}

void removeComments(char *testcaseFile, char *cleanFile) {
    FILE *inputFile = fopen(testcaseFile, "r");
    if (inputFile == NULL) {
        printf("Error opening input file\n");
        return;
    }

    FILE *outputFile = fopen(cleanFile, "w");
    if (outputFile == NULL) {
        printf("Error opening output file\n");
        return;
    }
    // Read the input file line by line
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, inputFile)) != -1) {
        if(line[0] == '%') {
            continue;
        }
        fprintf(outputFile, "%s", line);
    }
    fclose(inputFile);
    fclose(outputFile);
}
