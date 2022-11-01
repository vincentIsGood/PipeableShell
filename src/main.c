#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>
#include "cmdline.h"
#include "executils.h"
#include "utils.h"
#include "signals.h"
#include "jobs.h"

/**
 * @return break the user input loop or not (ie. exit or not)
 */
boolean handleParseResult(parse_result*);

/**
 * @return Array of command execution results, if gatherUsage 
 * is true. Null otherwise. Remember to free the 2d pointer array
 */
generic_2darray handleArbitraryCommands(traverse_info*, boolean gatherUsage);

char* readUserInput();
void cleanup(char* buf, parse_result);

int main(int argc, char* argv[]){
    registerSignalActions();
    initReport();

    boolean stopProgram = false;
    while(!stopProgram){
        print_raw("$$ pshell ## ");
        char* inputBuf = readUserInput();
        printDoneBgJobsReport();
        
        parse_result result = parseLine(inputBuf);
        if(result.argc > 0)
            stopProgram = handleParseResult(&result);
        cleanup(inputBuf, result);
    }
    cleanJobs();
    clearReport();
    return 0;
}

// -------------- implementation -------------- //
// Helpers
char* readUserInput(){
    char* buffer = malloc(1024*sizeof(char));
    fgets(buffer, 1024, stdin);
    buffer[strlen(buffer)-1] = 0; // delete '\n'
    return buffer;
}

void cleanup(char* inputBuf, parse_result result){
    freeParseResult(result);
    free(inputBuf);
}

// Handle parse results
boolean handleParseResult(parse_result* result){
    traverse_info info;
    info.currentIndex = 0;
    info.res = result;

    if(stringEquals("exit", result->args[0])){
        if(result->argc > 1){
            shellErr("\"exit\" with other arguments!!!");
            return false;
        }
        shellPrint("Terminated");
        return true;
    }else if(stringEquals("timeX", result->args[0])){
        // command check
        if(result->argc == 1){
            shellErr("\"timeX\" cannot be a standalone command");
            return false;
        }else if(stringEquals("&", result->args[result->argc-1])){
            shellErr("\"timeX\" cannot be run in background mode");
            return false;
        }
        skipNextToken(&info); // skip "timeX"

        // execute the real commands to be timed
        generic_2darray execResults = handleArbitraryCommands(&info, true);
        for(int i = 0; i < execResults.length; i++){
            exec_result* result = (exec_result*)execResults.array[i];
            println("(PID)%d  (CMD)%s    (user)%.3f s  (sys)%.3f s", 
                result->pid, getProperProgramName(result->programName), 
                result->usage.ru_utime.tv_sec + ((double)(result->usage.ru_utime.tv_usec))/1000000, 
                result->usage.ru_stime.tv_sec + ((double)(result->usage.ru_stime.tv_usec))/1000000);
            free(result->programName);
        }
        free2dPointer(execResults.array, execResults.length);
    }else{
        generic_2darray execResults = handleArbitraryCommands(&info, false);
        for(int i = 0; i < execResults.length; i++){
            exec_result* result = (exec_result*)execResults.array[i];
            free(result->programName);
        }
        free2dPointer(execResults.array, execResults.length);
    }
    return false;
}

generic_2darray handleArbitraryCommands(traverse_info* info, boolean gatherUsage){
    generic_2darray execResults;
    int execArraySize = 5;
    int execArrayI = 0;
    exec_result** execArray = malloc(execArraySize*sizeof(exec_result*));

    int p1ToP2Pipe[2];
    int p2ToP3Pipe[2];

    boolean pipeExists = false;
    boolean doInBackground = false;
    
    // Check if we can run a command in background or not
    parse_result* tokenizedResult = info->res;
    if(tokenizedResult->argc > 0){
        char* lastToken = tokenizedResult->args[tokenizedResult->argc-1];
        if(stringEquals("&", lastToken) &&
        tokenizedResult->argc > 1 && stringEquals("&", tokenizedResult->args[tokenizedResult->argc-2])){
            shellErr("'&' should not appear in the middle of the command line");
            goto returnArray;
        }
        if(stringEquals("&", lastToken)){
            free(lastToken);
            (tokenizedResult->argc)--;
            doInBackground = true;
        }
    }

    parse_result command;
    command.state = NORMAL_STATE;
    while(command.state != END_OF_ARRAY_REACHED 
    && command.state != UNEXPECTED_STATE){
        command = parseArgsForExec(info);
        command.getRusage = gatherUsage;
        if(command.errors)
            break;
        if(command.argc == 0){
            free(command.args);
            break;
        }
        if(command.state == AMPERSAND_AT_THE_END){
            free(command.args);
            doInBackground = false;
            shellErr("Unexpected error: Program should have parsed the ending ampersand in advance");
            break;
        }

        exec_result* execResult;
        if(doInBackground)
            command.backgroundMode = true;
        if(pipeExists){
            if(command.state == END_OF_ARRAY_REACHED){
                execResult = execWithOpenedPipe(command, p1ToP2Pipe, NULL);
            }else if(command.state == PIPE_ENCOUNTERED){
                execResult = execWithOpenedPipe(command, p1ToP2Pipe, p2ToP3Pipe);
                // swap the pipes (since p1ToP2Pipe is closed)
                p1ToP2Pipe[0] = p2ToP3Pipe[0];
                p1ToP2Pipe[1] = p2ToP3Pipe[1];
            }
        }else{
            if(command.state == END_OF_ARRAY_REACHED){
                execResult = simpleExecAndWait(command);
            }else if(command.state == PIPE_ENCOUNTERED){
                pipeExists = true;
                execResult = execWithNewPipe(command, p1ToP2Pipe);
            }
        }
        free(command.args);

        if(execResult != NULL){
            // print exit status of terminated child commands (after wait() from the exec() up there)
            char* exitStatusStr = WIFEXITED(execResult->exitStatus)? NULL : (WIFSTOPPED(execResult->exitStatus)? strsignal(WSTOPSIG(execResult->exitStatus)) : strsignal(WTERMSIG(execResult->exitStatus)));
            if(exitStatusStr != NULL)
                println("%s", exitStatusStr);

            execArray[execArrayI++] = execResult;
            if(execArrayI >= execArraySize-1)
                expand2dArray((void***)&execArray, &execArraySize);
        }
        
        if(command.state == PIPE_ENCOUNTERED)
            skipNextToken(info); // skip pipe
    }
returnArray:
    doInBackground = false; // reset
    execResults.array = (void**)execArray;
    execResults.length = execArrayI;
    return execResults;
}