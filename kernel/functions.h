#ifndef SIMPLE_EXAMPLE_FUNCTIONS_H
#define SIMPLE_EXAMPLE_FUNCTIONS_H

int listProcesses(char ** arguments, void * context);
int processStats(char ** arguments, void * context);
int printGlobalFileTable(char ** arguments, void * context);
int changeMultiprogrammingDeg(char ** arguments, void * context);
int killProcess(char ** arguments, void * context);
int pauseScheduling(char ** arguments, void * context);
int resumeScheduling(char ** arguments, void * context);

#endif //SIMPLE_EXAMPLE_FUNCTIONS_H
