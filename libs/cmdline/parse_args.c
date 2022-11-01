#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "cmdline.h"
#include "utils.h"


// Tried to use strsep, but it is quite limited. Old notes:
// stresep won't malloc new memory for tokens
// for(; (token = strsep(&line, " ")); arrayIndex++){
/**
 * Simple rules applied, create a token if we find special chars.
 * Technically, (double / single) quotes can be implemented as well.
 */
parse_result parseLine(char* line){
    parse_result result = {};
    if(strlen(line) == 0){
        result.argc = 0;
        result.args = NULL;
        return result;
    }
    int arrayIndex = 0;
    int arrSize = 5;
    char** resultArray = malloc(arrSize*sizeof(char*));
    char* token = allocEmptyStringWithSize(line);
    int tokenI = 0;
    for(int i = 0; line[i] != 0; i++){
        // encounter special chars to add tokens
        if(line[i] == ' ' || line[i] == '|' || line[i] == '&'){
            // do not let empty tokens get through
            if(strlen(token) > 0){
                // add previous token first
                token[tokenI] = 0; // null byte
                resultArray[arrayIndex++] = token;
                // reset token
                tokenI = 0;
                token = allocEmptyStringWithSize(line);
                if(arrayIndex >= arrSize-1)
                    expand2dArray((void***)&resultArray, &arrSize);
            }
            //     don't add the spaces
            if(line[i] == ' ')
                continue;

            // add the special char
            if(line[i] == '|') resultArray[arrayIndex++] = strdup("|");
            else if(line[i] == '&') resultArray[arrayIndex++] = strdup("&");
            if(arrayIndex >= arrSize-1)
                expand2dArray((void***)&resultArray, &arrSize);
            continue;
        }
        token[tokenI++] = line[i];
    }
    // add token once end-of-string is reached
    if(strlen(token) > 0){
        token[tokenI] = 0;
        resultArray[arrayIndex++] = token;
    }else{
        free(token);
    }
    result.argc = arrayIndex;
    result.args = resultArray;
    return result;
}

parse_result parseArgsForExec(traverse_info* info){
    parse_result result = {};
    int argvAllocSize = 5;
    int argvI = 0;
    char** argv = malloc(argvAllocSize*sizeof(char*));

    // TODO: move prints to a reasonable place.
    char* nextToken = getCurrentToken(info);
    for(; nextToken != NULL; nextToken = getNextToken(info)){
        if(stringEquals(nextToken, "|")){
            if((nextToken = peekNextToken(info)) == NULL){
                result.errors = DANGLING_PIPE_ERR;
                shellErr("cannot use dangling pipe");
                free(argv);
                return result;
            }
            if(stringEquals(nextToken, "|")){
                result.errors = CONSECUTIVE_PIPES_ERR;
                shellErr("should not have two consecutive | without in-between command");
                free(argv);
                return result;
            }
            result.state = PIPE_ENCOUNTERED;
            break;
        }else if(peekNextToken(info) != NULL && stringEquals(nextToken, "&")){
            result.errors = INVALID_AMPERSAND_POS_ERR;
            shellErr("'&' should not appear in the middle of the command line");
            break;
        }else if(peekNextToken(info) == NULL && stringEquals(nextToken, "&")){
            result.state = AMPERSAND_AT_THE_END;
            break;
        }
        argv[argvI++] = getCurrentToken(info);

        if(argvI >= argvAllocSize-1)
            expand2dArray((void***)&argv, &argvAllocSize);
    }
    if(nextToken == NULL)
        result.state = END_OF_ARRAY_REACHED;
    argv[argvI] = 0; // array is null terminated
    result.args = argv;
    result.argc = argvI;
    result.errors = 0;
    result.backgroundMode = false;
    result.getRusage = false;
    return result;
}

void freeParseResult(parse_result result){
    free2dPointer((void**)result.args, result.argc);
}