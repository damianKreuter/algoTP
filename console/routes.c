#include <stdio.h>
#include <commons/string.h>
#include "../easysocket/commonstructs.h"
#include "../easysocket/socketcontext.h"

/** This file contains the route handlers that will be called when a message reaches a socket **/



/**
 * Saves the received buffer of the program's output for later displaying (print command)
 * @params arguments [buffer]
 * @from KERNEL
 */
int programOutput(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
	logContext(socketContext, debug, "Echo endpoint, printing arguments");
	string_iterate_lines(arguments, (void (*)(char *)) puts);
    return 0;
}


/**
 * Response from the kernel with the PID
 * @params arguments [PID]
 * @from KERNEL
 */
int initResponse(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
	logContext(socketContext, debug, "Kernel response with the PID, yay");
	string_iterate_lines(arguments, (void (*)(char *)) puts);
    return 0;
}

