#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "lexerDef.h"

TwinBuffer createTwinBuffer(FILE *fp) {
    TwinBuffer tb = (TwinBuffer*)malloc(sizeof(TwinBuffer));
    tb->currentBuffer = tb->buffer1;
    tb->currentPos = 0;
    tb->bufferSize = 0;
    tb->fp = fp;
    return tb;
}

TwinBuffer getStream(FILE *fp) {
    // to be implemented
    static TwinBuffer tb = NULL;
    
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

TokenInfo getNextToken(TwinBuffer B) {
    int state = 0;
    TokenInfo token_to_return = malloc(sizeof(struct tokenInfo));
    // check if B->currentPos >= B->bufferSize and reload buffer if necessary
    char currentChar;
    int current_lexeme_length = 0;

    while (1) {
        currentChar = B->currentBuffer[B->currentPos];

        switch (state)
        {
        case 0: // START state
            if (currentChar == '\t' || currentChar == '\r' || currentChar == ' ') {
                state = 0;
            } else if (currentChar >= 40 && currentChar <= 47) { // [40-47] are ['(', ')', '*', '+', ',', '-', '.', '/']
                token_to_return->lexeme = malloc(sizeof(char));
                (token_to_return->lexeme)[0] = currentChar;
                token_to_return->lexemeLength = 1;
                state = (int) (currentChar - (char) 39);
            } else if (currentChar == ':' || currentChar == ';') {
                state = (int) (currentChar - ':' + (char) 9);
            } else if (currentChar == '[') {
                state = 11;
            } else if (currentChar == ']') {
                state = 12;
            } else if (currentChar == '~') {
                state = 13;
            } else if (currentChar == '&') {
                state = 14;
            } else if (currentChar == '@') {
                state = 15;
            } else if (currentChar == '=') {
                state = 16;
            } else if (currentChar >= 60 && currentChar <= 62) { // [60-61] are ['!', '<', '>']
                state = (int) (currentChar - 43);
            } else if (currentChar == '_') {
                state = 20;
            } else if (currentChar == '#') {
                state = 21;
            } else if (currentChar == '%') {
                state = 22;
            } else if (currentChar == '\n') {
                state = 23;
            } else if (currentChar >= 'b' && currentChar <= 'd') {
                state = 24;
            } else if (currentChar == 'a' || (currentChar >= 'e' && currentChar <= 'z')) {
                state = 25;
            } else if (currentChar >= '0' && currentChar <= '9') {
                state = 26;
            } else {
                // ERROR
            }

            break;

        case 1: // ( state
            token_to_return->token = TK_OP;
            return token_to_return;

        case 2: // ) state
            token_to_return->token = TK_CL;
            return token_to_return;

        case 3: // * state
            token_to_return->token = TK_MUL;
            return token_to_return;

        case 4: // + state
            token_to_return->token == TK_PLUS;
            return token_to_return;

        case 5: // , state
            token_to_return->token = TK_COMMA;
            return token_to_return;

        case 6: // - state
            token_to_return->token == TK_MINUS;
            return token_to_return;
        
        case 7: // . state
            token_to_return->token == TK_DOT;
            return token_to_return;

        case 8: // / state
            token_to_return->token == TK_DIV;
            return token_to_return;

        case 9: // : state
            token_to_return->token = TK_COLON;
            return token_to_return;

        case 10: // ; state
            token_to_return->token = TK_SEM;
            return token_to_return;

        case 11: // [
            token_to_return->token = TK_SQL;
            return token_to_return;

        case 12: // ]
            token_to_return->token = TK_SQR;
            return token_to_return;

        case 13: // ~
            token_to_return->token = TK_NOT;
            return token_to_return;

        case 14: // &
            if (currentChar == '&') {
                state = 27;
            } else {
                // ERROR
            }

            break;

        case 15: // @
            if (currentChar == '@') {
                state = 28;
            } else {
                // ERROR
            }

            break;

        case 16: // =
            if (currentChar == '=') {
                state = 29;
            } else {
                // ERROR
            }

            break;

        case 17: // !
            if (currentChar == '=') {
                state = 30;
            } else {
                // ERROR
            }

            break;

        case 18: // <
            if (currentChar == '=') {
                state = 31;
            } else if (currentChar == '-') {
                state = 32;
            } else {
                state = 33;
            }

            break;

        case 19: // >
            if (currentChar == '=') {
                state = 34;
            } else {
                state = 35;
            }

            break;

        case 20: // _
            if ((currentChar >= 'A' && currentChar <= 'Z') || (currentChar >= 'a' && currentChar <= 'z')) {
                state = 36;
            } else {
                // ERROR
            }

            break;

        case 21: // #
            if (currentChar >= 'a' && currentChar <= 'z') {
                state = 37;
            } else {
                // ERROR
            }

            break;

        case 22: // %
            if (currentChar == '\n') {
                state = 23;
            } else {
                state = 22;
            }

            break;

        case 23: // \n
            // increment line number
            state = 0;
            break;

        case 24: // [b-d]
            if (currentChar >= 'a' && currentChar <= 'z') {
                state = 38;
            } else if (currentChar >= '2' && currentChar <= '7') {
                state = 39;
            } else {
                state = 40;
            }

            break;

        case 25: // [a|e-z]
            if (currentChar >= 'a' && currentChar <= 'z') {
                state = 25;
            } else {
                state = 40;
            }

            break;

        case 26: // [0-9]
            if (currentChar >= '0' && currentChar <= '9') {
                state = 26;
            } else if (currentChar == '.') {
                state = 41;
            } else {
                state = 42;
            }

            break;

        case 27: // &&
            if (currentChar == '&') {
                state = 43;
            } else {
                // ERROR
            }

            break;

        case 28: // @@
            if (currentChar == '@') {
                state = 44;
            } else {
                // ERROR
            }

            break;

        case 29: // ==
            
        
        default:
            break;
        }
    
        ++(B->currentPos);
    }

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
