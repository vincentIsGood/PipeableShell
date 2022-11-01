#include "cmdline.h"

char* getPrevToken(traverse_info* info){
    return info->res->args[info->currentIndex-1];
}

char* getCurrentToken(traverse_info* info){
    return info->res->args[info->currentIndex];
}

char* getNextToken(traverse_info* info){
    if(info->currentIndex + 1 >= info->res->argc)
        return NULL;
    return info->res->args[++(info->currentIndex)];
}

void skipNextToken(traverse_info* info){
    info->currentIndex++;
}

char* peekNextToken(traverse_info* info){
    if(info->currentIndex + 1 >= info->res->argc)
        return NULL;
    return info->res->args[info->currentIndex+1];
}