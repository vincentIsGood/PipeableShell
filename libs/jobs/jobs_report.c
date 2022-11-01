#include "jobs.h"
#include "utils.h"

int reportAllocatedSize = 5;
generic_2darray bgJobsReport;

void initReport(){
    bgJobsReport.length = 0;
    bgJobsReport.array = malloc(reportAllocatedSize*sizeof(void*));
}

void addDoneBgJobsReport(char* report){
    bgJobsReport.array[bgJobsReport.length++] = report;
    if(bgJobsReport.length >= reportAllocatedSize-1)
        expand2dArray((void***)&bgJobsReport.array, &reportAllocatedSize);
}

void printDoneBgJobsReport(){
    for(int i = 0; i < bgJobsReport.length; i++)
        println(bgJobsReport.array[i]);
    clearReport();
}

void clearReport(){
    for(int i = 0; i < bgJobsReport.length; i++)
        free(bgJobsReport.array[i]);
    bgJobsReport.length = 0;
}

void freeReport(){
    free2dPointer(bgJobsReport.array, bgJobsReport.length);
}