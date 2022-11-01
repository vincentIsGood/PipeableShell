#include "jobs.h"
#include "utils.h"

int size = 5;
simple_job** jobPids = NULL; // element is null if it is deleted

// find the first NULL element
void addJob(int pid, char* programName){
    if(jobPids == NULL){
        // init array with 0s //memset
        jobPids = calloc(size, sizeof(simple_job*));
    }

    simple_job* job = malloc(sizeof(simple_job));
    job->pid = pid;
    job->programName = strdup(programName);
    for(int i = 0; i < size; i++){
        if(jobPids[i] == NULL){
            jobPids[i] = job;
            return;
        }else if(i == size-1){
            expand2dArray((void***)&jobPids, &size);
            jobPids[i] = job;
            return;
        }
    }
}

// null if not found
simple_job* getJob(int pid){
    if(jobPids == NULL) return NULL;
    for(int i = 0; i < size; i++){
        if(jobPids[i] != NULL && jobPids[i]->pid == pid)
            return jobPids[i];
    }
    return NULL;
}

// setting it null
void deleteJob(int pid){
    if(jobPids == NULL) return;
    for(int i = 0; i < size; i++){
        if(jobPids[i] != NULL && jobPids[i]->pid == pid){
            free(jobPids[i]->programName);
            free(jobPids[i]);
            jobPids[i] = NULL;
            return;
        }
    }
}

void cleanJobs(){
    if(jobPids == NULL) return;
    for(int i = 0; i < size; i++){
        if(jobPids[i] != NULL)
            free(jobPids[i]->programName);
    }
    free2dPointer((void**)jobPids, size);
}