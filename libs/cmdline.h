#ifndef CMDLINE_H
#define CMDLINE_H
#include "utils.h"

// parse_result.errors
#define DANGLING_PIPE_ERR 1
#define CONSECUTIVE_PIPES_ERR 2
#define INVALID_AMPERSAND_POS_ERR 3
#define PARSER_ERR 10

// parse_result.state
#define NORMAL_STATE -1
#define UNEXPECTED_STATE 0
#define PIPE_ENCOUNTERED 1
#define END_OF_ARRAY_REACHED 2
#define AMPERSAND_AT_THE_END 3

typedef struct parse_result{
    unsigned int argc;   // # of args
    char** args;         // parsed result (args)
    unsigned int errors; // if error > 0, error occured
    unsigned int state;  // state given by parser after parsing

    // options to be passed to executils
    boolean getRusage;        // default to false (ie. 0)
    boolean backgroundMode;   // default to false (ie. 0)
} parse_result;

// Used to traverse a parse_result
typedef struct traverse_info{
    parse_result* res;
    int currentIndex;
} traverse_info;

/**
 * Create tokens as we walk through the line. 
 * New strings are allocated to the heap for each token
 */
parse_result parseLine(char* line);

/**
 * Take output from #parseLine and get the next command
 * @param info [mutate] used to indicate where the parser 
 * should start parsing. Once pipe is encountered, it 
 * returns immediately. **Remember to skip the pipe 
 * manually**
 * 
 * @return result for one command. Its "args" needs to be 
 * freed manually. But, its elements are not "malloc"ed. 
 * If an error occured, nothing needs to be freed.
 */
parse_result parseArgsForExec(traverse_info* info);

char* getPrevToken(traverse_info*);

char* getCurrentToken(traverse_info*);

/**
 * @return null if an end is reach
 */
char* getNextToken(traverse_info*);

void skipNextToken(traverse_info*);

/**
 * @return null if an end is reach
 */
char* peekNextToken(traverse_info*);

/**
 * free args from parse_result
 */
void freeParseResult(parse_result);

#endif