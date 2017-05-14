#include "job.h"

vector <Job> jobList;
Job *current_job = NULL;
Job *previous_job = NULL;

int getIndex()
{
    int index;

    // find min available index
    for (index = 1; index < MAX_BACKGROUND_JOB; index++)
    {
        bool found = true;

        for (const auto &job : jobList)
        {
            if (job.index == index)
            {
                found = false;
                break;
            }
        }

        if (found) break;
    }

    return index;
}

void updateCurrentJob()
{
    current_job = previous_job = NULL;

    // find last and second last stoppped job
    for (int i = jobList.size() - 1; i >= 0; i--) 
    {
        if (current_job != NULL && previous_job != NULL)
            break;
        if (jobList[i].status == SUSPENDED && current_job == NULL)
            current_job = &jobList[i];
        else if (jobList[i].status == SUSPENDED && previous_job == NULL)
            previous_job = &jobList[i];
    }

    // find last and second last running job
    for (int i = jobList.size() - 1; i >= 0; i--) 
    {
        if (current_job != NULL && previous_job != NULL)
            break;
        if (jobList[i].status == RUNNING && current_job == NULL)
            current_job = &jobList[i];
        else if (jobList[i].status == RUNNING && previous_job == NULL)
            previous_job = &jobList[i];
    }
}

void addJob(Job job)
{
    job.index = getIndex();
    jobList.push_back(job);

    updateCurrentJob();
}

void removeJob(int index)
{
    for (int i = 0; i < jobList.size(); i++)
    {
        if (jobList[i].index == index)
        {
            jobList.erase(jobList.begin() + i);
            updateCurrentJob();

            return;
        }
    }
}

void printLastJobPid()
{
    Job &lastJob = jobList.back();

    printf("[%d] ", lastJob.index);

    for (const auto &command : lastJob.commands)
        printf("%d ", command.pid);

    printf("\n");
}

// return job's index if it's the last process of job, else return 0 or return -1 if not found
int processDone(pid_t pid)
{
    for (int i = 0; i < jobList.size(); i++)
    {
        auto command = find_if(jobList[i].commands.begin(), jobList[i].commands.end(),
                               [=](const Command& cmd) { return cmd.pid == pid; });

        // found
        if (command != jobList[i].commands.end())
        {
            command->status = DONE;

            if (command->cmdline == jobList[i].commands.back().cmdline)
                return jobList[i].index;

            return 0;
        }
    }

    return -1;
}

// if index = 0, print all jobs, else print job which index = index
void printJob(int index)
{
    // print all jobs sort by index
    // copy to avoid mess up the original order
    vector <Job> list;
    list.assign(jobList.begin(), jobList.end());

    sort(list.begin(), list.end());

    int current_job_index = (current_job == NULL ? -1 : current_job->index);
    int previous_job_index = (previous_job == NULL ? -1 : previous_job->index);

    for (const auto &job : list)
    {
        if (index == 0 || job.index == index)
        {
            printf("[%d]  %c ", job.index, job.index == current_job_index ? '+' :
                                          job.index == previous_job_index ? '-' : ' ');

            for (int i = 0; i < job.commands.size(); i++)
            {
                if (i != 0) cout << "      ";

                printf("%d %-11s", job.commands[i].pid, job.commands[i].status.c_str());
                cout << job.commands[i] << endl;
            }
        }
    }
}

