#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <sys/wait.h>
#include "Command.h"

using namespace std;

#define MAXLINE 15000
#define PROMPT "shell-prompt$ "

extern char **environ;

int main(int argc, char *argv[])
{
    string line;

    cout << PROMPT;

    while (getline(cin, line))
    {
        stringstream ss(line);
        bool redirect_input = false, redirect_output = false;
        string redirect_input_file, redirect_output_file, str;
        vector <string> input;
        vector <string> command;
        vector <Command> job;

        while (ss >> str)
            input.push_back(str);

        // push empty string to indicate end of line
        input.push_back("");

        for (int i = 0; i < input.size(); i++)
        {
            if (input[i] == "exit") {
                exit(0);
            }
            else if (input[i] == "printenv")
            {
                // print specific environment variable
                if(input.size() == 3) {
                    char *name = (char *) input[++i].c_str();
                    char *value = getenv(name);

                    if(value)
                        cout << name << "=" << getenv(name) << endl;
                    else
                        cout << name << ": unset environment variable" << endl;
                }
                // print all environment variable
                else if(input.size() == 2) {
                    for(int i = 0; environ[i] != NULL; i++) {
                        cout << environ[i] << endl;
                    }
                }
            }
            else if (input[i] == "export")
            {
                string str = input[++i];
                size_t pos = str.find("=");
                string name = str.substr(0, pos);
                string value = str.substr(pos+1, str.size());

                setenv(name.c_str(), value.c_str(), 1);
            }
            else if (input[i] == "unset")
            {
                string name = input[++i];
                unsetenv(name.c_str());
            }
            else if (input[i] == "|" || input[i] == "") {
                Command cmd;
                cmd.command = command;
                if(redirect_input) {
                    cmd.redirect_input = true;
                    cmd.redirect_input_file = redirect_input_file;
                }
                if(redirect_output) {
                    cmd.redirect_output = true;
                    cmd.redirect_output_file = redirect_output_file;
                }
                job.push_back(cmd);

                // reset
                redirect_input = false;
                redirect_output = false;
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
            else {
                command.push_back(input[i]);
            }
        }

        // set up pipes
        if(job.size() > 1) {
            for (int i = 0; i < job.size() - 1; i++)
            {
                int pipefd[2];

                pipe(pipefd);

                Command::pipes.push_back(pipefd[0]);
                Command::pipes.push_back(pipefd[1]);

                job[i].writefd = pipefd[1];
                job[i + 1].readfd = pipefd[0];
            }
        }

//        cout << "======================================================" << endl;
//        for(int i = 0; i < job.size(); i++)
//            job[i].log();
//        cout << "======================================================" << endl;

        // execute command
        for(int i = 0; i < job.size(); i++) {
            if(fork() == 0)
                job[i].execute();
        }

        // parent close all pipes
        Command::closeAllPipes();

        // parent wait for child
        for(int i = 0; i < job.size(); i++) {
            int status;
            wait(&status);
        }

        Command::pipes.clear();

        cout << PROMPT;
    }

    return 0;
}