#include <stdlib.h>
#include <stdio.h>

void expand2dArray(void*** array, int* allocatedSize){
    void** newArr = realloc(*array, (*allocatedSize += 5)*sizeof(void*));
    if(newArr != NULL) *array = newArr;
    else exit(1);
}

void free2dPointer(void** array, int size){
    for(int i = 0; i < size; i++){
        if(*(array + i) != NULL)
            free(*(array + i)); // take element from array (eg. char*)
    }
    if(array != NULL)
        free(array);
}