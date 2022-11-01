#ifndef JOBS_H
#define JOBS_H
#include <stdlib.h>
#include <string.h>

typedef struct simple_job{
    int pid;
    char* programName;
} simple_job;

void addJob(int pid, char* programName);

simple_job* getJob(int pid);

void deleteJob(int pid);

void cleanJobs();

// ----- jobs report ----- //
void initReport();

void addDoneBgJobsReport(char* report);

void printDoneBgJobsReport();

void clearReport();

#endif