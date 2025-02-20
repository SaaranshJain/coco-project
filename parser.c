#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

FirstAndFollow* ComputeFirstAndFollowSets (Grammar G)
{
    return NULL;
}

void createParseTable(FirstAndFollow F, Table T)
{
}

ParseTree parseInputSourceCode(char *testcaseFileName, Table T)
{
    FILE* testcasefile = fopen(testcaseFileName, "r");
    TwinBuffer buffer = getStream(testcasefile);
    TokenInfo token;
    LookupTable lt = create_lookup_table();

    while((token = getNextToken(buffer, lt)) != NULL) {
        
    }
}

void printParseTree(ParseTree PT, char *outfile)
{
}
