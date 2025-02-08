#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include "lexer.h"
#include "lexerDef.h"

TwinBuffer getStream(FILE *fp) {
    TwinBuffer tb = (TwinBuffer) malloc(sizeof(struct twinBuffer));
    tb->currentBuffer = tb->buffer1;
    tb->currentPos = 0;
    tb->bufferSize = fread(tb->currentBuffer, 1, TWIN_BUFFER_INDIVIDUAL_BUFFER_SIZE, fp);
    tb->fp = fp;
    return tb;
}

bool refresh_buffer(TwinBuffer B) {
    B->currentBuffer = (B->currentBuffer == B->buffer1) ? B->buffer2: B->buffer1;
    B->currentPos = 0;
    B->bufferSize = fread(B->currentBuffer, 1, TWIN_BUFFER_INDIVIDUAL_BUFFER_SIZE, B->fp);

    return !(B->bufferSize == 0);
}

TokenInfo getNextToken(TwinBuffer B) {
    int state = 0;
    char currentChar;
    
    int current_lexeme_cap = 32;
    TokenInfo token_to_return = malloc(sizeof(struct tokenInfo));
    token_to_return->lexeme = malloc(32 * sizeof(char));
    token_to_return->lexemeLength = 0;

    while ((B->currentPos < B->bufferSize) || refresh_buffer(B)) { // use short circuit evaluation, as long as pos < size it will continue, if not it will refresh
        if (token_to_return->lexemeLength >= current_lexeme_cap) {
            token_to_return->lexeme = realloc(token_to_return->lexeme, 2*current_lexeme_cap);
            current_lexeme_cap = current_lexeme_cap + current_lexeme_cap;
        }

        currentChar = B->currentBuffer[B->currentPos];
        (token_to_return->lexeme)[(token_to_return->lexemeLength)++] = currentChar;

        switch (state)
        {
        case 0: // START state
            if (currentChar == '\t' || currentChar == '\r' || currentChar == ' ') {
                state = 0;
            } else if (currentChar >= 40 && currentChar <= 47) { // [40-47] are ['(', ')', '*', '+', ',', '-', '.', '/']
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
            token_to_return->token = TK_EQ;
            return token_to_return;

        case 30: // !=
            token_to_return->token = TK_NE;
            return token_to_return;

        case 31: // <=
            token_to_return->token = TK_LE;
            return token_to_return;

        case 32: // <-
            if (currentChar == '-') {
                state = 45;
            } else {
                state = 46;
            }

            break;

        case 33: // < other
            --(token_to_return->lexemeLength); --(B->currentPos);
            token_to_return->token = TK_LT;
            return token_to_return;

        case 34: // >=
            token_to_return->token = TK_GE;
            return token_to_return;

        case 35: // > other
            --(token_to_return->lexemeLength); --(B->currentPos);
            token_to_return->token = TK_GT;
            return token_to_return;

        case 36: // _[a-zA-Z]
            if ((currentChar >= 'A' && currentChar <= 'Z') || (currentChar >= 'a' && currentChar <= 'z')) {
                state = 36;
            } else if (currentChar >= '0' && currentChar <= '9') {
                state = 47;
            } else {
                state = 48;
            }

            break;

        case 37: // #[a-z]
            if (currentChar >= 'a' && currentChar <= 'z') {
                state = 37;
            } else {
                state = 49;
            }

            break;

        case 38: // [b-d][a-z]
            if (currentChar >= 'a' && currentChar <= 'z') {
                state = 38;
            } else {
                state = 40;
            }

            break;

        case 39: // [b-d][2-7]
            if (currentChar >= 'b' && currentChar <= 'd') {
                state = 39;
            } else if (currentChar >= '2' && currentChar <= '7') {
                state = 50;
            } else {
                state = 51;
            }

            break;

        case 40: // [b-d] other
            --(token_to_return->lexemeLength); --(B->currentPos);
            // lookup
            token_to_return->token = TK_FIELDID;
            return token_to_return;

        case 41: // [0-9].
            if (currentChar >= '0' && currentChar <= '9') {
                state = 52;
            } else {
                state = 53;
            }

            break;

        case 42: // [0-9] other
            --(token_to_return->lexemeLength); --(B->currentPos);
            token_to_return->token = TK_NUM;
            token_to_return->lexeme = atoi(token_to_return->lexeme);
            return token_to_return;

        case 43: // &&&
            token_to_return->token = TK_AND;
            return token_to_return;
            
        case 44: // @@@
            token_to_return->token = TK_OR;
            return token_to_return;

        case 45: // <--
            if (currentChar == '-') {
                state = 54;
            } else {
                // ERROR
            }

            break;

        case 46: // <- other
            token_to_return->lexemeLength -= 2; B->currentPos -= 2;
            token_to_return->token = TK_LT;
            return token_to_return;

        case 47: // _[a-zA-Z][0-9]
            if (currentChar >= '0' && currentChar <= '9') {
                state = 47;
            } else {
                state = 48;
            }

            break;

        case 48: // _[a-zA-Z][0-9]? other
            --(token_to_return->lexemeLength); --(B->currentPos);
            // lookup
            token_to_return->token = TK_FUNID;
            return token_to_return;

        case 49: // #[a-z] other
            --(token_to_return->lexemeLength); --(B->currentPos);
            // lookup
            token_to_return->token = TK_RUID;
            return token_to_return;

        case 50: // [b-d][2-7][2-7]
            if (currentChar >= '2' && currentChar <= '7') {
                state = 50;
            } else {
                state = 51;
            }

            break;

        case 51: // [b-d][2-7] other
            --(token_to_return->lexemeLength); --(B->currentPos);
            // lookup
            token_to_return->token = TK_ID;
            return token_to_return;

        case 52: // [0-9].[0-9]
            if (currentChar >= '0' && currentChar <= '9') {
                state = 55;
            } else {
                // ERROR
            }

            break;

        case 53: // [0-9]. other
            token_to_return->lexemeLength -= 2;
            B->currentPos -= 2;
            token_to_return->token = TK_NUM;
            token_to_return->lexeme = atoi(token_to_return->lexeme);
            return token_to_return;

        case 54: // <---
            token_to_return->token = TK_ASSIGNOP;
            return token_to_return;

        case 55: // [0-9].[0-9][0-9]
            if (currentChar == 'E') {
                state = 56;
            } else {
                state = 57;
            }

            break;

        case 56: // [0-9].[0-9][0-9]E
            if ((currentChar == '+') || (currentChar == '-')) {
                state = 58;
            } else if (currentChar >= '0' && currentChar <= '9') {
                state = 59;
            } else {
                // ERROR
            }

            break;

        case 57: // [0-9].[0-9][0-9] other
            --(token_to_return->lexemeLength); --(B->currentPos);
            token_to_return->token = TK_RNUM;
            token_to_return->lexeme = atof(token_to_return->lexeme);
            return token_to_return;

        case 58: // [0-9].[0-9][0-9]E[+-]
            if (currentChar >= '0' && currentChar <= '9') {
                state = 59;
            } else {
                // ERROR
            }

            break;

        case 59: // [0-9].[0-9][0-9]E[+-][0-9]
            if (currentChar >= '0' && currentChar <= '9') {
                state = 60;
            } else {
                // ERROR
            }

            break;

        case 60: // [0-9].[0-9][0-9]E[+-][0-9][0-9]
            token_to_return->token = TK_RNUM;
            token_to_return->lexeme = atof(token_to_return->lexeme);
            return token_to_return;

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
