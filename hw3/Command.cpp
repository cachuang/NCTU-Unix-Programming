#include "Command.h"

vector <int> Command::pipes;

Command::Command()
{
    readfd = STDIN_FILENO;
    writefd = STDOUT_FILENO;
    redirect_output = false;
    redirect_input = false;
}

void Command::execute()
{
    if(redirect_input) {
        FILE *fp = fopen(redirect_input_file.c_str(), "r");
        if(fp)
            dup2(fileno(fp), STDIN_FILENO);
        else {
            cerr << redirect_input_file << ": no such file or directory" << endl;
            exit(-1);
        }
    }
    else
        dup2(readfd, STDIN_FILENO);

    if(redirect_output) {
        FILE *fp = fopen(redirect_output_file.c_str(), "w");
        if(fp)
            dup2(fileno(fp), STDOUT_FILENO);
        else {
            cerr << redirect_output_file << ": Invalid file name" << endl;
            exit(-1);
        }
    }
    else
        dup2(writefd, STDOUT_FILENO);

    Command::closeAllPipes();

    // convert string to char ** and do word expansion
    vector <char *> cmd;
    for(int i = 0; i < this->command.size(); i++) {
        wordexp_t p;
        char **w;

        wordexp(this->command[i].c_str(), &p, 0);
        w = p.we_wordv;
        for (int j = 0; j < p.we_wordc; j++)
            cmd.push_back(strdup(w[j]));
        wordfree(&p);
    }
    cmd.push_back(NULL);

    if(execvp(cmd[0], &cmd[0]) < 0) {
        cerr << cmd[0] << ": command not found" << endl;
        exit(-1);
    }
}

void Command::log()
{
    for(int i = 0; i < command.size(); i++)
        cout << command[i] << " ";
    cout << ": readfd: " << this->readfd << ", writefd: " << this->writefd;
    cout << ", pipes: ";
    for(int i = 0; i < this->pipes.size(); i++)
        cout << this->pipes[i] << ", ";
    cout << endl;
}

void Command::closeAllPipes()
{
    for(int i = 0; i < Command::pipes.size(); i++)
        close(Command::pipes[i]);
}



