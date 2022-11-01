#include <string.h>
#include "utils.h"

/**
 * Allocate string of size longString
 */
char* allocEmptyStringWithSize(char* longString){
    char* token = strdup(longString);
    token[0] = 0;
}

boolean stringEquals(char* str1, char* str2){
    return strcmp(str1, str2) == 0;
}

int lastIndexOf(char* str, char target){
    int lastSeenTargetPos = -1;
    for(int i = 0; str[i] != 0; i++){
        if(str[i] == target) lastSeenTargetPos = i;
    }
    return lastSeenTargetPos;
}

char* getProperProgramName(char* programName){
    int indexOfSlash = lastIndexOf(programName, '/');
    if(indexOfSlash == -1) indexOfSlash = 0;
    else indexOfSlash++;
    return programName + indexOfSlash;
}