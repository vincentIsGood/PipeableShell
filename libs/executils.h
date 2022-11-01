#ifndef EXECUTILS_H
#define EXECUTILS_H
#include <sys/time.h>
#include <sys/resource.h>
#include "cmdline.h"
#include "utils.h"

typedef struct exec_result{
    struct rusage usage;
    char* programName;        // remember to free program
    int pid;
    int exitStatus;
} exec_result;

/// ------- app & ipc ------- ///
/**
 * @param command should come from #parseArgsForExec
 * @return remember to free it (esp. char* program)
 */
exec_result* simpleExecAndWait(parse_result command);

/**
 * @param newPipe [mutate] new pipe is set here. You are required to close 
 * it yourself
 * @return remember to free it  (esp. char* program)
 */
exec_result* execWithNewPipe(parse_result command, int* newPipe);

/**
 * Child's stdout is wired to newPipe, read it from the newPipe
 * @param prevPipe [mod] pipe from previous command. Will be closed here. 
 * @param newPipe [nullable; mutate] new pipe is set here. You are required 
 * to close it yourself
 * @return remember to free it  (esp. char* program)
 */
exec_result* execWithOpenedPipe(parse_result command, int* prevPipe, int* newPipe);

int getRecentPid();

void clearedToExecute();

/// Example:
/*
    char* args[] = {
        "ls", "-la", "|", "grep", "il"
    };

    parse_result tmp;
    tmp.argc = 5;
    tmp.args = args;

    traverse_info info;
    info.currentIndex = 0;
    info.res = &tmp;

    int p1ToP2Pipe[2];
    execWithNewPipe(parseArgsForExec(&info), p1ToP2Pipe);
    skipNextToken(&info); // skip pipe
    
    // int p2ToP3Pipe[2];
    execWithOpenedPipe(parseArgsForExec(&info), p1ToP2Pipe, NULL);

    // close(p2ToP3Pipe[0]);
    // close(p2ToP3Pipe[1]);
    println("test done!");
*/

#endif