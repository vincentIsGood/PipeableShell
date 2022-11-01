#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include "executils.h"
#include "utils.h"
#include "jobs.h"

// Signals
void handleSignal(int signum){
    if(signum == SIGINT){
        int pid = getRecentPid();
        if(pid == -1)
            print_raw("\n$$ pshell ## ");
    }else if(signum == SIGKILL){
        print_raw(strsignal(SIGKILL));
    }else if(signum == SIGUSR1){
        clearedToExecute();
    }else if(signum == SIGCHLD){
        int pid;
        int status;
        simple_job* job;
        while((pid = waitpid(-1, &status, WNOHANG)) > 0){
            if((job = getJob(pid)) != NULL){
                char* exitStatus = WIFEXITED(status)? "Done" : (WIFSTOPPED(status)? strsignal(WSTOPSIG(status)) : strsignal(WTERMSIG(status)));
                char* reportString = malloc(1024*sizeof(char));
                snprintf(reportString, 1024, "[%d] %s %s", job->pid, getProperProgramName(job->programName), exitStatus);
                addDoneBgJobsReport(reportString);
                deleteJob(pid);
            }
        }
    }
}

void registerSignalActions(){
    signal(SIGINT, handleSignal);
    signal(SIGKILL, handleSignal);
    signal(SIGUSR1, handleSignal);
    signal(SIGCHLD, handleSignal);
}