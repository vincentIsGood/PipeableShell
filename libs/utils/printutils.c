#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "consts.h"

void print_raw(char* str){
    write(0, str, strlen(str));
}

void println(char* format, ...){
    if(format == NULL)
        return;
    va_list args;
    va_start(args, format); // args, then the last name of the arg in function definition

    int bufferSize = strlen(format) + 2; // include "\n" + NULL
    char finalFormat[bufferSize];
    snprintf(finalFormat, bufferSize, "%s\n", format);
    vprintf(finalFormat, args);

    va_end(args);
}

void shellPrint(char* format, ...){
    if(format == NULL)
        return;
    va_list args;
    va_start(args, format); // args, then the last name of the arg in function definition

    int bufferSize = strlen(format) + strlen("pshell: ") + 2; // include "\n" + NULL
    char finalFormat[bufferSize];
    snprintf(finalFormat, bufferSize, "pshell: %s\n", format);
    vfprintf(stdout, finalFormat, args);

    va_end(args);
}

void shellErr(char* format, ...){
#if PRINT_ERROR
    if(format == NULL)
        return;
    va_list args;
    va_start(args, format); // args, then the last name of the arg in function definition

    int bufferSize = strlen(format) + strlen("pshell: ") + 2; // include "\n" + NULL
    char finalFormat[bufferSize];
    snprintf(finalFormat, bufferSize, "pshell: %s\n", format);
    // vfprintf(stderr, finalFormat, args);
    vfprintf(stdout, finalFormat, args);

    va_end(args);
#endif
}

void print2dPointer(char** arr, int size){
    for(int i = 0; i < size; i++)
        println("%s", *(arr + i));
}