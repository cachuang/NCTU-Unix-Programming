#ifndef UNTITLED_COMMAND_H
#define UNTITLED_COMMAND_H

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
    Command();
    void execute();
    void log();
    static void closeAllPipes();

    int readfd;
    int writefd;
    bool redirect_input;
    bool redirect_output;
    string redirect_input_file;
    string redirect_output_file;
    vector <string> command;
    static vector <int> pipes;
};

#endif //UNTITLED_COMMAND_H
