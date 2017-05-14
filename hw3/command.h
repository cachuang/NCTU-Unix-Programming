#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>
#include <string>
#include <vector>

#include <unistd.h>
#include <string.h>
#include <wordexp.h>

using namespace std;

class Command
{
public:
    friend ostream& operator << (ostream& os, const Command& cmd);

    Command();
    void execute();
    static void closeAllPipes();

    int readfd;
    int writefd;
    int pid;
    int pgid;
    bool redirect_input;
    bool redirect_output;
    string status;
    string redirect_input_file;
    string redirect_output_file;
    vector <string> cmdline;
    static vector <int> pipes;
};

#endif // COMMAND_H
