//
// Created by Alejo on 6/3/2017.
//

#ifndef SIMPLE_EXAMPLE_MEMORYROUTES_H
#define SIMPLE_EXAMPLE_MEMORYROUTES_H

#include "../easysocket/commonstructs.h"


/**
 * Define the endpoints
 */

// KERNEL
#define INIT_PROCESS_ENDPOINT "init"
#define ASSIGN_PAGE_ENDPOINT "assign"
#define END_PROCESS_ENDPOINT "end"

// CPU
#define READ_MEMORY_ENDPOINT "read"
#define WRITE_MEMORY_ENDPOINT "write"


/**
 * Now the functions
 */
int initProgramMemory(char ** arguments, t_socketContext * socketContext);
int assignPages(char ** arguments, t_socketContext * socketContext);
int endProgramMemory(char ** arguments, t_socketContext * socketContext);
int readMemory(char ** arguments, t_socketContext * socketContext);
int writeMemory(char ** arguments, t_socketContext * socketContext);


#endif //SIMPLE_EXAMPLE_MEMORYROUTES_H
