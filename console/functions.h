//
// Created by Alejo on 6/2/2017.
//

#ifndef SIMPLE_EXAMPLE_CONSOLECOMMANDS_H_H
#define SIMPLE_EXAMPLE_CONSOLECOMMANDS_H_H

#define DEFAULT_PROGRAM_PID "DEFAULT"

int runProgram(char ** arguments, void * context);
int printProgramOutput(char ** arguments, void * context);
int killProgram(char ** arguments, void * context);

#endif //SIMPLE_EXAMPLE_CONSOLECOMMANDS_H_H
