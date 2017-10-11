#ifndef SIMPLE_EXAMPLE_KERNELROUTES_H
#define SIMPLE_EXAMPLE_KERNELROUTES_H

#include "../easysocket/commonstructs.h"
#include "kernel.h"


/**
 * Define the endpoints
 */

// CONSOLE
#define INIT_PROCESS_ENDPOINT "init"

// CPU
#define ALLOCATE_MEMORY_ENDPOINT "init"
#define FREE_MEMORY_ENDPOINT "init"
#define OPEN_FILE_ENDPOINT "init"
#define READ_FILE_ENDPOINT "init"
#define WRITE_FILE_ENDPOINT "init"
#define CLOSE_FILE_ENDPOINT "init"


/**
 * Now the functions
 */
int initProgram(char ** arguments, t_socketContext * socketContext);
int allocateMemory(char ** arguments, t_socketContext * socketContext);
int freeMemory(char ** arguments, t_socketContext * socketContext);
int openFile(char ** arguments, t_socketContext * socketContext);
int readFile(char ** arguments, t_socketContext * socketContext);
int writeFile(char ** arguments, t_socketContext * socketContext);
int closeFile(char ** arguments, t_socketContext * socketContext);


#endif //SIMPLE_EXAMPLE_KERNELROUTES_H
