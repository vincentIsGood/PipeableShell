#ifndef VIN_UTILS_H
#define VIN_UTILS_H
#include <stdlib.h>

typedef unsigned int boolean;
#define true  1
#define false 0

typedef struct generic_2darray{
    void** array;
    int length;
} generic_2darray;

/// ------- debugging ------- ///
void print_raw(char* str);

/**
 * Simply print text with new line with current pid
 */
void println(char* format, ...);

void shellPrint(char* format, ...);

void shellErr(char* format, ...);

void print2dPointer(char** arr, int size);

/// ------- strings ------- ///
char* allocEmptyStringWithSize(char* longString);

boolean stringEquals(char* str1, char* str2);

/**
 * @return -1 if not found
 */
int lastIndexOf(char*, char);

char* getProperProgramName(char* programName);

/// ------- memory ------- ///

void expand2dArray(void*** array, int* allocatedSize);

/* Example
char** strings = malloc(3*sizeof(char*));
strings[0] = strdup("send help");
strings[1] = strdup("send help");
strings[2] = strdup("send help");
free2dPointer(strings, 3);
*/
/**
 * @param size length of the wrapper array (eg. [ele1, ele2, ele3] has a size of 3)
 */
void free2dPointer(void** array, int size);

#endif