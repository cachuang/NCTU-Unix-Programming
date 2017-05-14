#ifndef BUILTINS_H
#define BUILTINS_H

#include <iostream>
#include <csignal>

#include <sys/wait.h>

#include "job.h"

extern char **environ;

using namespace std;

void printenv();
void printenv(const char *name);
void exportenv(const char *name, const char *value);
void unset(const char *name);
void jobs();
void fg();
void bg();

#endif // BUILTINS_H
