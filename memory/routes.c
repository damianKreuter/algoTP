/**
 * This file contains the route handlers that will be called when a message reaches a socket
 */

#include "../easysocket/commonstructs.h"



/*
 * KERNEL ROUTES
 */


/**
 * Assigns the first pages for a process
 * @params arguments [PID, amount]
 * @from
 */
int initProgramMemory(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
    return 0;
}


/**
 * Assigns more pages to a running process
 * @params arguments [PID, amount]
 * @from KERNEL
 */
int assignPages(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
    return 0;
}


/**
 * Frees all memory assigned to a process
 * @params arguments [PID]
 * @from KERNEL
 */
int endProgramMemory(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
    return 0;
}



/*
 * CPU ROUTES
 * // TODO: and maybe also kernel?
 */


/**
 * Reads the required bytes from memory and returns
 * @params arguments [PID, page, offset, size]
 * @from CPU
 */
int readMemory(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
    
    return 0;
}


/**
 * Writes the provided bytes to memory at page->offset
 * @params arguments [PID, page, offset, size, buffer]
 * @from CPU
 */
int writeMemory(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
    return 0;
}