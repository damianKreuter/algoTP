/**
 * This file contains the route handlers that will be called when a message reaches a socket
 */

#include <commons/string.h>
#include "../easysocket/libeasysocket.h"
#include "../easysocket/pcb.h"
#include "kernel.h"
#include "../console/routes.h"

/*
 * CONSOLE ROUTES
 */


/**
 * Receives the initial payload to initialize a program
 * TODO:Fix desc
 * @param arguments [script]
 * @from CONSOLE
 */
int initProgram(char ** arguments, t_socketContext * socketContext) {
	/*char * aggs[2];
	aggs[0] = "Conectado a kernel";
	aggs[1] = NULL;
	logContext(socketContext, info, "Console said '%s';Waving back", arguments[0]);
	sendMessagesInContext(socketContext, "echo", aggs);*/
	t_kernel * kernel = socketContext->processContext;

	// TODO: Create PCB from arguments - in progress
	// Create the PCB from arguments
	t_pcb * pcb = newPcb();
//	pcb->script = arguments[0];

	// Add it to the new process list, which should also set its PID
	addProcessToQueue(kernel, NEW, pcb);

	// TODO: Request MEMORY -> assign the necessary pages for the code & stack, also send the whole code - in progress
	// If there isn't enough space, return the rejection to the console (w/ error code)

	// Return the PID to console
	sendMessageInContext(socketContext, INIT_PROGRAM_RESPONSE, string_itoa(pcb->PID));

	return 0;
}




/*
 * MEMORY LAYER ROUTES
 */


/**
 * Syscall from the cpu to request the allocation of n bytes
 * @from CPU
 */
int allocateMemory(char ** arguments, t_socketContext * socketContext) {
	// TODO: Complete
	/**
	* Should manage these pages such that processes can ask for them and free them at any time
	* Can fit more than one block of memory in a page (e.g. page 512B, req for 50B -> ok, new req 100B -> same block)
	* To keep block consistency, we will fill the page with metadata, request :
	* 	1) Check if the request exceeds the max size (PAGE_SIZE - 2 * sizeof(HeapMetadata)) => Pg. 13 for expl.
	* 		1.a) If there isn't, return the rejection code
	* 	2) If no page was previously assigned to the process, request a new one from memory
	* 		2.a) In which case initialize it with a HeapMetadata structure at the beginning
	* 	3) Check if there is enough space in the page to allocate the request
	* 		3.a) If there isn't execute 2.a and continue
	* 	4) Allocate the memory, update the metadata struct (from free to used, and the size used)
	* 	5) Return the pointer immediately following the metadata
	* Should implement a table (commons.set) maintaining the rel between PCB (PID), page no. and its available size
	* If the size requested does not fit in the page, request a new table from memory
	* Returns the logical address for the contiguous block of memory to the running program (the cpu process)
	* After everything, develop an algorithm to fix internal fragmentation
	*/
	return 0;
}


/**
 * Syscall from the cpu to free a previously alloc'd address
 * @from CPU
 */
int freeMemory(char ** arguments, t_socketContext * socketContext) {
	// TODO: Complete
	/**
	 * 1) Request the necessary things from the memory process
	 * 2) Search for the structure corresponding to the received address arg
	 * 3) If all is ok free it (delete the struct)
	 *  3.a) Else segfault, end program
	 * 4) Return status
	 */
	return 0;
}



/**
 * FS LAYER ROUTES
 */

/**
 * Receives string to print from a console process
 * @from CONSOLE
 */
//int print(char ** arguments, t_socketContext * socketContext){
//    // TODO: Complete, also I think this shouldn't exists and only writeFile is expected
//	/*
//	 * Uses the FD 1, that will be reserved for this purpose (i.e. logging)
//	 */
//	return 0;
//}



/**
 * Opens a file, generates both a global and process FD and returns the latter
 * @args path, permissions ("crw")
 * @from CPU
 * @returns FD
 */
int openFile(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
	// Basically only generates the entries in the process file table (PFT)
	return 0;
}


/**
 * Reads a file (or part of it) and returns the contents
 * @args FD, offset, size
 * @from CPU
 * @returns content
 */
int readFile(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
	// I'm not sure if it's supposed to return the content or a memory address to it
	return 0;
}


/**
 * Writes the provided contents to a file
 * @args FD, offset, size?, content
 * @from CPU
 * @returns success|error
 */
int writeFile(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
	return 0;
}


/**
 * Closes a file (destroys the FD - Path entry in the PFT)
 * @from CPU
 */
int closeFile(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
    // Also if this makes the global file table counter reach 0, remove that entry as well
	return 0;
}












