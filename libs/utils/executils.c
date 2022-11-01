#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include "utils.h"
#include "cmdline.h"
#include "executils.h"
#include "consts.h"
#include "jobs.h"

// SIGUSR1 signal received
static boolean clearanceReceived = false;
static int recentPid = -1; // -1 running process is done

void sendClearance(int pid){
#if ENABLE_SIGUSR_FEATURE
    // sleep(1); // test SIGUSR1
    kill(pid, SIGUSR1);
#endif
}

void blockUntilClearance(){
#if ENABLE_SIGUSR_FEATURE
    // while(!clearanceReceived) sleep(9999);
    while(!clearanceReceived) pause();
    clearanceReceived = false;
#endif
}

void execCommand(parse_result command){
    blockUntilClearance();
    if(execvp(command.args[0], command.args) == -1){
        shellErr("'%s': %s", command.args[0], strerror(errno));
        exit(1);
    }
}

exec_result* simpleExecAndWait(parse_result command){
    exec_result* result = malloc(sizeof(exec_result));
    result->programName = strdup(command.args[0]);
    struct rusage usage;
    int exitStatus;
    int pid;
    if((pid = fork()) == 0){
        // Get error from "strerror(errno)"
        execCommand(command);
    }else{
        // parent
        recentPid = pid;
        if(command.backgroundMode)
            addJob(pid, command.args[0]);
        sendClearance(pid);

        if(!command.backgroundMode){
            if(command.getRusage){
                wait4(pid, &exitStatus, 0, &usage);
            }else waitpid(pid, &exitStatus, 0);
        }

        // after waiting, reset the pid
        if(recentPid == pid)
            recentPid = -1;
    }
    result->usage = usage;
    result->exitStatus = exitStatus;
    result->pid = pid;
    return result;
}

exec_result* execWithNewPipe(parse_result command, int* newPipe){
    pipe(newPipe);
    int* p1ToP2Pipe = newPipe;

    exec_result* result = malloc(sizeof(exec_result));
    result->programName = strdup(command.args[0]);
    struct rusage usage;
    int exitStatus;
    int pid;
    if((pid = fork()) == 0){ // proc1
        close(p1ToP2Pipe[0]);   // reading not needed
        dup2(p1ToP2Pipe[1], 1); // pipe write <- stdout (ie. proc1 -> p1ToP2Pipe)
        execCommand(command);
    }else{
        // parent
        recentPid = pid;
        if(command.backgroundMode)
            addJob(pid, command.args[0]);
        sendClearance(pid);

        if(!command.backgroundMode){
            if(command.getRusage){
                wait4(pid, &exitStatus, 0, &usage);
            }else waitpid(pid, &exitStatus, 0);
        }

        // after waiting, reset the pid
        if(recentPid == pid)
            recentPid = -1;
    }
    result->usage = usage;
    result->exitStatus = exitStatus;
    result->pid = pid;
    return result;
}

exec_result* execWithOpenedPipe(parse_result command, int* prevPipe, int* newPipe){
    if(newPipe != NULL)
        pipe(newPipe);
    int* p1ToP2Pipe = prevPipe;
    int* p2ToP3Pipe = newPipe;

    exec_result* result = malloc(sizeof(exec_result));
    result->programName = strdup(command.args[0]);
    struct rusage usage;
    int exitStatus;
    int pid;
    if((pid = fork()) == 0){ // proc2
                            close(p1ToP2Pipe[1]);   // writing to old pipe is not needed
        if(newPipe != NULL) close(p2ToP3Pipe[0]);   // reading from proc3 is not needed
                            dup2(p1ToP2Pipe[0], 0); // pipe read <- stdin (ie. p1ToP2Pipe -> proc2)
        if(newPipe != NULL) dup2(p2ToP3Pipe[1], 1); // pipe write <- stdout (ie. proc2 -> p2ToP3Pipe)
        execCommand(command);
    }else{
        // parent
        // close previous pipe
        close(p1ToP2Pipe[0]);
        close(p1ToP2Pipe[1]);
        recentPid = pid;
        if(command.backgroundMode)
            addJob(pid, command.args[0]);
        sendClearance(pid);

        if(!command.backgroundMode){
            if(command.getRusage){
                wait4(pid, &exitStatus, 0, &usage);
            }else waitpid(pid, &exitStatus, 0);
        }

        // after waiting, reset the pid
        if(recentPid == pid)
            recentPid = -1;
    }
    result->usage = usage;
    result->exitStatus = exitStatus;
    result->pid = pid;
    return result;
}

int getRecentPid(){
    return recentPid;
}

void clearedToExecute(){
    clearanceReceived = true;
}