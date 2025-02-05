#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "lexerDef.h"

TwinBuffer* createTwinBuffer(FILE *fp) {
    TwinBuffer *tb = (TwinBuffer*)malloc(sizeof(TwinBuffer));
    tb->currentBuffer = tb->buffer1;
    tb->currentPos = 0;
    tb->bufferSize = 0;
    tb->fp = fp;
    return tb;
}

TwinBuffer* getStream(FILE *fp) {
    // to be implemented
    static TwinBuffer *tb = NULL;
    
    if (tb == NULL) {
        tb = createTwinBuffer(fp);
    }
    
    if (tb->currentPos >= tb->bufferSize) {
        // Switch buffers
        tb->currentBuffer = (tb->currentBuffer == tb->buffer1) ? tb->buffer2 : tb->buffer1;
        tb->bufferSize = fread(tb->currentBuffer, 1, BUFFER_SIZE, tb->fp);
        tb->currentPos = 0;
        
        if (tb->bufferSize == 0) {
            // End of file reached
            return NULL;
        }
    }
    // return tb->fp;
    return tb;
}

tokenInfo getNextToken(TwinBuffer B) {
    
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
