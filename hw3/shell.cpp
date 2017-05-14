#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <sys/wait.h>

#include "command.h"
#include "builtins.h"
#include "job.h"

using namespace std;

#define PROMPT "shell-prompt$ "

void sigchld_handler(int signal)
{
    int jobIndex;
    pid_t pid;

    while ((pid = waitpid(-1, NULL, WNOHANG)))
    {
        if ((jobIndex = processDone(pid)) > 0)
        {
            // Job done
            cout << endl;
            printJob(jobIndex);
            cout << PROMPT;
            fflush(stdout);

            removeJob(jobIndex);
        }

        if (errno == ECHILD)  break;
    }

    return;
}

int main(int argc, char *argv[])
{
    string line;

    signal(SIGCHLD, sigchld_handler);
    signal(SIGTSTP, SIG_IGN);

    while (true)
    {
        bool redirect_input = false, redirect_output = false, background = false, builtin = false;
        string redirect_input_file, redirect_output_file, str;
        vector <string> input;
        vector <string> command;
        Job job;

        cout << PROMPT;

        if(!getline(cin, line))
            break;

        stringstream ss(line);
        while (ss >> str)
            input.push_back(str);

        // push empty string to indicate end of line
        input.push_back("");

        // parse input
        for (int i = 0; i < input.size(); i++)
        {
            if (input[i] == "exit") {
                exit(0);
            }
            else if (input[i] == "printenv") {
                builtin = true;
                if(input.size() == 3)
                    printenv(input[++i].c_str());
                else if(input.size() == 2)
                    printenv();
            }
            else if (input[i] == "export") {
                builtin = true;
                string str = input[++i];
                size_t pos = str.find("=");
                string name = str.substr(0, pos);
                string value = str.substr(pos+1, str.size());

                exportenv(name.c_str(), value.c_str());
            }
            else if (input[i] == "unset") {
                builtin = true;
                unset(input[++i].c_str());
            }
            else if (input[i] == "jobs") {
                builtin = true;
                jobs();
            }
            else if (input[i] == "fg") {
                builtin = true;
                fg();
            }
            else if (input[i] == "bg") {
                builtin = true;
                bg();
            }
            else if (input[i] == "|" || input[i] == "") {
                Command cmd;

                cmd.cmdline = command;

                if (redirect_input) {
                    cmd.redirect_input = true;
                    cmd.redirect_input_file = redirect_input_file;
                }

                if (redirect_output) {
                    cmd.redirect_output = true;
                    cmd.redirect_output_file = redirect_output_file;
                }

                job.commands.push_back(cmd);

                // reset
                redirect_input = redirect_output = false;
                command.clear();
            }
            else if (input[i] == "<") {
                redirect_input = true;
                redirect_input_file = input[++i];
            }
            else if (input[i] == ">") {
                redirect_output = true;
                redirect_output_file = input[++i];
            }
            else if (input[i] == "&") {
                background = true;
            }
            else {
                command.push_back(input[i]);
            }
        }

        if(builtin)
            continue;

        // set up pipes
        if (job.commands.size() > 1)
        {
            for (int i = 0; i < job.commands.size() - 1; i++)
            {
                int pipefd[2];

                pipe(pipefd);

                Command::pipes.push_back(pipefd[0]);
                Command::pipes.push_back(pipefd[1]);

                job.commands[i].writefd = pipefd[1];
                job.commands[i+1].readfd = pipefd[0];
            }
        }

        // execute command and set process group
        pid_t pgid = 0;
        for (int i = 0; i < job.commands.size(); i++)
        {
            pid_t pid = fork();

            // first command of the pipeline will be the pgid
            if (i == 0) 
            {
                pgid = pid;
                job.pgid = pgid;
            }
            job.commands[i].pid = pid;
            job.commands[i].pgid = pgid;
            job.commands[i].status = RUNNING;

            // set all processes in pipeline with same pgid
            if (pid == 0) 
            {
                signal(SIGTSTP,SIG_DFL);
                signal(SIGCHLD,SIG_DFL);
                setpgid(0, pgid);

                job.commands[i].execute();
            }
            else 
                setpgid(pid, pgid);
        }

        // parent close all pipes
        Command::closeAllPipes();
        Command::pipes.clear();

        if (!background)
        {
            // make child process become the foreground process
            // signal(SIGCHLD, SIG_DFL);
            signal(SIGTTOU, SIG_IGN);
            tcsetpgrp(STDIN_FILENO, pgid);

            // parent wait for child
            for (int i = 0; i < job.commands.size(); i++)
            {
                int status = 0;

                // set WUNTRACED to make waitpid return if child is stopped
                pid_t pid = waitpid(-pgid, &status, WUNTRACED);

                if (WIFSTOPPED(status))   // stopped command
                {
                    auto command = find_if(job.commands.begin(), job.commands.end(),
                                           [=](const Command& cmd) { return cmd.pid == pid; });

                    command->status = SUSPENDED;
                    job.status = SUSPENDED;
                    job.remain_command_count = job.commands.size() - i;
                    addJob(job);

                    break;
                }
            }

            // parent become foreground process
            tcsetpgrp(STDIN_FILENO, getpid());
            signal(SIGTTOU, SIG_DFL);
            // signal(SIGCHLD, sigchld_handler);
        }
        // running in background
        else
        {
            job.status = RUNNING;
            addJob(job);
            printLastJobPid();
        }
    }

    return 0;
}