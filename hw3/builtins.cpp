#include "builtins.h"

// print all environment variable
void printenv()
{
    for (int i = 0; environ[i] != NULL; i++)
        cout << environ[i] << endl;
}

// print specific environment variable
void printenv(const char *name)
{
    char *value = getenv(name);

    if (value)
        cout << name << "=" << getenv(name) << endl;
    else
        cout << name << ": undefined environment variable" << endl;
}

void exportenv(const char *name, const char *value)
{
    setenv(name, value, 1);
}

void unset(const char *name)
{
    unsetenv(name);
}

void jobs()
{
    if (jobList.empty())
    {
        cout << "jobs: no current job" << endl;
        return;
    }

    printJob(0);
}

void fg()
{
    if (current_job == NULL)
    {
        cout << "fg: no current job" << endl;
        return;
    }

    kill(current_job->pgid, SIGCONT);

    for (auto &command : current_job->commands)
       command.status = CONTINUED;

    printJob(current_job->index);

    void (*sigchld_handler)(int) = signal(SIGCHLD, SIG_DFL);
    signal(SIGTTOU, SIG_IGN);
    tcsetpgrp(STDIN_FILENO, current_job->pgid);

    bool done = true;
    for (int i = 0; i < current_job->remain_command_count; i++)
    {
        int status = 0;
        pid_t pid = waitpid(-current_job->pgid, &status, WUNTRACED);

        if (WIFSTOPPED(status))   // stopped command
        {
            auto command = find_if(current_job->commands.begin(), current_job->commands.end(),
                                   [=](const Command& cmd) { return cmd.pid == pid; });

            command->status = SUSPENDED;
            current_job->status = SUSPENDED;
            current_job->remain_command_count -= i;
            done = false;

            break;
        }
    }

    tcsetpgrp(STDIN_FILENO, getpid());
    signal(SIGTTOU, SIG_DFL);
    signal(SIGCHLD, sigchld_handler);

    if (done)
        removeJob(current_job->index);
}

void bg()
{
    if (current_job == NULL)
    {
        cout << "bg: no current job" << endl;
        return;
    }

    if (current_job->status == SUSPENDED)
    {
        kill(current_job->pgid, SIGCONT);

        for (auto &command : current_job->commands)
            command.status = CONTINUED;

        printJob(current_job->index);
    }
    else
        cout << "bg: job already running in background" << endl;
}