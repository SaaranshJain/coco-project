#include <stdio.h>
#include <sys/types.h>
#include <string.h>
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
        char *commentStart = strchr(line, '%');
        if (commentStart != NULL) {
            *commentStart = '\0'; // Terminate the line at the start of the comment
        }
        if (line[0] == '\0') continue;
        // Remove trailing newline character if present
        size_t lineLength = strlen(line);
        if (lineLength > 0 && line[lineLength - 1] == '\n') {
            line[lineLength - 1] = '\0';
        }
        fprintf(outputFile, "%s\n", line);
    }
    fclose(inputFile);
    fclose(outputFile);
}
