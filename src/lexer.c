#include "lexer.h"
#include "lookup.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

char *ENUM_NAME_FROM_VALUE[NUM_TERMINALS] = {
    "TK_DOLLAR", "TK_ASSIGNOP", "TK_COMMENT", "TK_FIELDID",
    "TK_ID", "TK_NUM", "TK_RNUM", "TK_FUNID", "TK_RUID",
    "TK_WITH", "TK_PARAMETERS", "TK_END", "TK_WHILE",
    "TK_UNION", "TK_ENDUNION", "TK_DEFINETYPE", "TK_AS",
    "TK_TYPE", "TK_MAIN", "TK_GLOBAL", "TK_PARAMETER",
    "TK_LIST", "TK_SQL", "TK_SQR", "TK_INPUT",
    "TK_OUTPUT", "TK_INT", "TK_REAL", "TK_COMMA",
    "TK_SEM", "TK_COLON", "TK_DOT", "TK_ENDWHILE",
    "TK_OP", "TK_CL", "TK_IF", "TK_THEN",
    "TK_ENDIF", "TK_READ", "TK_WRITE", "TK_RETURN",
    "TK_PLUS", "TK_MINUS", "TK_MUL", "TK_DIV",
    "TK_CALL", "TK_RECORD", "TK_ENDRECORD", "TK_ELSE",
    "TK_AND", "TK_OR", "TK_NOT", "TK_LT",
    "TK_LE", "TK_EQ", "TK_GT", "TK_GE",
    "TK_NE", "ERROR"};

TwinBuffer getStream(FILE *fp) {
    TwinBuffer tb = (TwinBuffer)malloc(sizeof(struct twinBuffer));
    tb->currentBuffer = tb->buffer1;
    tb->currentPos = 0;
    tb->bufferSize = fread(tb->currentBuffer, 1, TWIN_BUFFER_INDIVIDUAL_BUFFER_SIZE, fp);
    tb->fp = fp;
    tb->line = 1;
    return tb;
}

bool refresh_buffer(TwinBuffer B) {
    B->currentBuffer = (B->currentBuffer == B->buffer1) ? B->buffer2 : B->buffer1;
    B->currentPos = 0;
    B->bufferSize = fread(B->currentBuffer, 1, TWIN_BUFFER_INDIVIDUAL_BUFFER_SIZE, B->fp);
    return !(B->bufferSize == 0);
}

TokenInfo return_with_lookup(LookupTable lt, TokenInfo curr_token, int line) {
    LookupTableNode searched_node = search(lt, curr_token->lexeme, curr_token->lexemeLength);
    if (searched_node != NULL) {
        curr_token->token = searched_node->token;
    } else {
        insert(lt, curr_token->lexeme, curr_token->lexemeLength, curr_token->token, line);
    }

    return curr_token;
}

TokenInfo getNextToken(TwinBuffer B, LookupTable lt) {
    enum STATE state = START;
    char currentChar;
    int end_flag = 2;

    int current_lexeme_cap = 32;
    TokenInfo token_to_return = malloc(sizeof(struct tokenInfo));
    token_to_return->lexeme = malloc(32 * sizeof(char));
    memset(token_to_return->lexeme, '\0', 32);
    token_to_return->lexemeLength = 0;

    while ((B->currentPos < B->bufferSize) || refresh_buffer(B) || end_flag) { // use short circuit evaluation, as long as pos < size it will continue, if not it will refresh
        if (token_to_return->lexemeLength >= current_lexeme_cap) {
            token_to_return->lexeme = realloc(token_to_return->lexeme, 2 * current_lexeme_cap);
            memset(token_to_return->lexeme + current_lexeme_cap, '\0', current_lexeme_cap);
            current_lexeme_cap = current_lexeme_cap + current_lexeme_cap;
        }

        if ((B->currentPos >= B->bufferSize) && end_flag) {
            --end_flag;
            currentChar = -1;
        } else {
            currentChar = B->currentBuffer[(B->currentPos)++];
            (token_to_return->lexeme)[(token_to_return->lexemeLength)++] = currentChar;
        }

        switch (state) {
        case START:
            if (currentChar == '\t' || currentChar == '\r' || currentChar == ' ') {
                state = START;
                token_to_return->lexeme[--(token_to_return->lexemeLength)] = '\0';
            } else if (currentChar >= 40 && currentChar <= 47) { // [40-47] are ['(', ')', '*', '+', ',', '-', '.', '/']
                int tokens[8] = {TK_OP, TK_CL, TK_MUL, TK_PLUS, TK_COMMA, TK_MINUS, TK_DOT, TK_DIV};
                token_to_return->token = tokens[currentChar - 40];
                return token_to_return;
            } else if (currentChar == ':') {
                token_to_return->token = TK_COLON;
                return token_to_return;
            } else if (currentChar == ';') {
                token_to_return->token = TK_SEM;
                return token_to_return;
            } else if (currentChar == '[') {
                token_to_return->token = TK_SQL;
                return token_to_return;
            } else if (currentChar == ']') {
                token_to_return->token = TK_SQR;
                return token_to_return;
            } else if (currentChar == '~') {
                token_to_return->token = TK_NOT;
                return token_to_return;
            } else if (currentChar == '&') {
                state = FIRST_AND;
            } else if (currentChar == '@') {
                state = FIRST_OR;
            } else if (currentChar == '=') {
                state = FIRST_EQ;
            } else if (currentChar == '!') {
                state = FIRST_NOT;
            } else if (currentChar == '<') {
                state = FIRST_LT;
            } else if (currentChar == '>') {
                state = FIRST_GT;
            } else if (currentChar == '_') {
                state = UNDERSCORE;
            } else if (currentChar == '#') {
                state = HASHTAG;
            } else if (currentChar == '%') {
                state = PERCENT;
                token_to_return->lexeme[--(token_to_return->lexemeLength)] = '\0';
            } else if (currentChar == '\n') {
                ++(B->line);
                state = START;
                token_to_return->lexeme[--(token_to_return->lexemeLength)] = '\0';
            } else if (currentChar >= 'b' && currentChar <= 'd') {
                state = FIRST_ID;
            } else if (currentChar == 'a' || (currentChar >= 'e' && currentChar <= 'z')) {
                state = FIELD_ID;
            } else if (currentChar >= '0' && currentChar <= '9') {
                state = FIRST_DIGIT;
            } else {
                if (currentChar != -1) {
                    fprintf(stderr, "Line %d\n\tUnexpected character: %c\n\n", B->line, currentChar);
                    (token_to_return->lexeme)[token_to_return->lexemeLength] = '\0';
                    token_to_return->token = ERROR;
                    return token_to_return;
                }

                state = START;
            }

            break;

        case FIRST_AND:
            if (currentChar == '&') {
                state = SECOND_AND;
            } else {
                fprintf(stderr, "Line %d\n\tExpected: '&', Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            break;

        case FIRST_OR:
            if (currentChar == '@') {
                state = SECOND_OR;
            } else {
                fprintf(stderr, "Line %d\n\tExpected: '@', Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            break;

        case FIRST_EQ:
            if (currentChar == '=') {
                token_to_return->token = TK_EQ;
                return token_to_return;
            } else {
                fprintf(stderr, "Line %d\n\tExpected: '=', Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            break;

        case FIRST_NOT:
            if (currentChar == '=') {
                token_to_return->token = TK_NE;
                return token_to_return;
            } else {
                fprintf(stderr, "Line %d\n\tExpected: '=', Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            break;

        case FIRST_LT:
            if (currentChar == '=') {
                token_to_return->token = TK_LE;
                return token_to_return;
            } else if (currentChar == '-') {
                state = LT_MINUS;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = TK_LT;
                return token_to_return;
            }

            break;

        case FIRST_GT:
            if (currentChar == '=') {
                token_to_return->token = TK_GE;
                return token_to_return;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = TK_GT;
                return token_to_return;
            }

            break;

        case UNDERSCORE:
            if ((currentChar >= 'A' && currentChar <= 'Z') || (currentChar >= 'a' && currentChar <= 'z')) {
                state = UNDERSCORE_LETTER;
            } else {
                fprintf(stderr, "Line %d\n\tExpected: lowercase/uppercase alphabet, Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            break;

        case HASHTAG:
            if (currentChar >= 'a' && currentChar <= 'z') {
                state = HASHTAG_LETTER;
            } else {
                fprintf(stderr, "Line %d\n\tExpected: lowercase alphabet, Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            break;

        case PERCENT:
            if (currentChar == '\n') {
                ++(B->line);
                state = START;
            } else {
                state = PERCENT;
            }

            token_to_return->lexeme[--(token_to_return->lexemeLength)] = '\0';
            break;

        case FIRST_ID:
            if (currentChar >= 'a' && currentChar <= 'z') {
                state = FIELD_ID;
            } else if (currentChar >= '2' && currentChar <= '7') {
                state = SECOND_ID;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = TK_FIELDID;
                return return_with_lookup(lt, token_to_return, B->line);
            }

            break;

        case FIELD_ID:
            if (currentChar >= 'a' && currentChar <= 'z') {
                state = FIELD_ID;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = TK_FIELDID;
                return return_with_lookup(lt, token_to_return, B->line);
            }

            break;

        case FIRST_DIGIT:
            if (currentChar >= '0' && currentChar <= '9') {
                state = FIRST_DIGIT;
            } else if (currentChar == '.') {
                state = DIGIT_DOT;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = TK_NUM;
                token_to_return->lexemeI = atoi(token_to_return->lexeme);
                return token_to_return;
            }

            break;

        case SECOND_AND:
            if (currentChar != '&') {
                fprintf(stderr, "Line %d\n\tExpected: '&', Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            token_to_return->token = TK_AND;
            return token_to_return;

        case SECOND_OR:
            if (currentChar != '@') {
                fprintf(stderr, "Line %d\n\tExpected: '@', Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            token_to_return->token = TK_OR;
            return token_to_return;

        case LT_MINUS:
            if (currentChar == '-') {
                state = ASSIGNOP;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= 1 + (currentChar != -1)] = '\0';
                B->currentPos -= 1 + (currentChar != -1);
                token_to_return->token = TK_LT;
                return token_to_return;
            }

            break;

        case UNDERSCORE_LETTER:
            if ((currentChar >= 'A' && currentChar <= 'Z') || (currentChar >= 'a' && currentChar <= 'z')) {
                state = UNDERSCORE_LETTER;
            } else if (currentChar >= '0' && currentChar <= '9') {
                state = FUNID;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = TK_FUNID;
                return return_with_lookup(lt, token_to_return, B->line);
            }

            break;

        case HASHTAG_LETTER:
            if (currentChar >= 'a' && currentChar <= 'z') {
                state = HASHTAG_LETTER;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = TK_RUID;
                return return_with_lookup(lt, token_to_return, B->line);
            }

            break;

        case SECOND_ID:
            if (currentChar >= 'b' && currentChar <= 'd') {
                state = SECOND_ID;
            } else if (currentChar >= '2' && currentChar <= '7') {
                state = NUMBER_ID;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = TK_ID;
                return return_with_lookup(lt, token_to_return, B->line);
            }

            break;

        case DIGIT_DOT:
            if (currentChar >= '0' && currentChar <= '9') {
                state = FIRST_FLDIG;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= 1 + (currentChar != -1)] = '\0';
                B->currentPos -= 1 + (currentChar != -1);
                token_to_return->token = TK_NUM;
                token_to_return->lexemeI = atoi(token_to_return->lexeme);
                return token_to_return;
            }

            break;

        case ASSIGNOP:
            if (currentChar != '-') {
                fprintf(stderr, "Line %d\n\tExpected: '-', Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            token_to_return->token = TK_ASSIGNOP;
            return token_to_return;

        case FUNID:
            if (currentChar >= '0' && currentChar <= '9') {
                state = FUNID;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = TK_FUNID;
                return return_with_lookup(lt, token_to_return, B->line);
            }

            break;

        case NUMBER_ID:
            if (currentChar >= '2' && currentChar <= '7') {
                state = NUMBER_ID;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = TK_ID;
                return return_with_lookup(lt, token_to_return, B->line);
            }

            break;

        case FIRST_FLDIG:
            if (currentChar >= '0' && currentChar <= '9') {
                state = SECOND_FLDIG;
            } else {
                fprintf(stderr, "Line %d\n\tExpected: a digit, Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            break;

        case SECOND_FLDIG:
            if (currentChar == 'E') {
                state = FL_EXP;
            } else {
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = TK_RNUM;
                token_to_return->lexemeF = atof(token_to_return->lexeme);
                return token_to_return;
            }

            break;

        case FL_EXP:
            if ((currentChar == '+') || (currentChar == '-')) {
                state = FL_EXP_SIGNED;
            } else if (currentChar >= '0' && currentChar <= '9') {
                state = FL_EXP_COMPLETE;
            } else {
                fprintf(stderr, "Line %d\n\tExpected: signed/unsigned exponent, Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            break;

        case FL_EXP_SIGNED:
            if (currentChar >= '0' && currentChar <= '9') {
                state = FL_EXP_COMPLETE;
            } else {
                fprintf(stderr, "Line %d\n\tExpected: a digit, Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            break;

        case FL_EXP_COMPLETE:
            if (currentChar < '0' || currentChar > '9') {
                fprintf(stderr, "Line %d\n\tExpected: a digit, Received: '%c'\n", B->line, currentChar);
                (token_to_return->lexeme)[token_to_return->lexemeLength -= (currentChar != -1)] = '\0';
                B->currentPos -= (currentChar != -1);
                token_to_return->token = ERROR;
                return token_to_return;
            }

            token_to_return->token = TK_RNUM;
            token_to_return->lexemeF = atof(token_to_return->lexeme);
            return token_to_return;

        default:
            break;
        }
    }

    return NULL;
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

    char line[2048];
    while (fgets(line, sizeof(line), inputFile)) {
        char *commentStart = strchr(line, '%');
        if (commentStart != NULL) {
            *commentStart = '\0'; // Terminate the line at the start of the comment
        }
        if (line[0] == '\0')
            continue;
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
