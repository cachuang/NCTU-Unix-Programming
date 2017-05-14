#ifndef JOB_H
#define JOB_H

#include <vector>
#include <algorithm>

#include "command.h"

#define MAX_BACKGROUND_JOB 1000

#define RUNNING 	"running"
#define SUSPENDED 	"suspended"
#define CONTINUED 	"continued"
#define DONE 		"done"

struct Job
{
    int index;
    int pgid;
    int remain_command_count;
    string status;
    vector <Command> commands;

    bool operator < (const Job& job) const { return index < job.index; }
};

extern vector <Job> jobList;
extern Job *current_job;
extern Job *previous_job;

int getIndex();
void addJob(Job job);
void removeJob(int index);
void updateCurrentJob();
int processDone(pid_t pid);
void printLastJobPid();
void printJob(int index);

#endif // JOB_H