/** This file contains the command handlers that can be executed from the main process **/

#include <stdio.h>
#include <commons/log.h>
#include <string.h>
#include <stdlib.h>
#include "main.h"
#include "functions.h"
#include "../kernel/routes.h"
#include "../easysocket/utils.h"

#define ANSISOP_HEADER "#!/usr/bin/ansisop"




/**
 * Utils
 */

bool isAnsisopScript(char * script) {
	return !strncmp(script, ANSISOP_HEADER, strlen(ANSISOP_HEADER));
}



/**
 * Commands
 */


/**
 * Runs an ANSISOP script
 * @param arguments [path]
 * @param context
 * @return
 */
int runProgram(char ** arguments, void * context){
	// Name arguments and context
	t_commandsContext * commandsContext = (t_commandsContext *) context;
	char * path = arguments[0];
	if(path == NULL){
		log_info(commandsLog, "You must specify a path to an ANSISOP script");
		return 0;
	}

	// Read the file
	log_debug(commandsLog, "Running script, path: '%s'", arguments[0]);
	char * script = readFileIntoBuffer(path);

	// Check that it's an ANSISOP script
	if(script ==  NULL || !isAnsisopScript(script)){
		log_warning(commandsLog, "The specified path does not contain an ANSISOP script");
		return 0;
	}

	// Check that the last program has already been assigned a PID
	if(setHasSocket(commandsContext->socketSet, DEFAULT_PROGRAM_PID)){
		log_warning(commandsLog, "The last program started has not been assigned a PID yet, retry in a few seconds");
	}

	log_debug(commandsLog, "All done, now send script to kernel");
	log_debug(commandsLog, "Connecting to kernel...");

	// Create config for the new socket connection to the kernel
	t_socketConfig kernelConfig = {
			.address =  commandsContext->kernelIP,
			.port =     commandsContext->kernelPort,
			.routes =   commandsContext->kernelRoutes,
			.name =     DEFAULT_PROGRAM_PID,
			.logLevel = commandsContext->loggingLevel
	};

	// Create the client socket and add it to the socketSet
	createClientSocket(&kernelConfig, commandsContext->socketSet, context);

	// Now that we are connected, send the program for initialization
	sendMessageToSocket(commandsContext->socketSet, DEFAULT_PROGRAM_PID, INIT_PROCESS_ENDPOINT, script);
	log_info(commandsLog, "Sent script '%s' to kernel, wait for PID", path);

	// Free what needs to be freed
	free(script);

	return 0;
}


/**
 * Kills a running ANSISOP script
 * @param arguments [pid]
 * @param context
 * @return
 */
int killProgram(char ** arguments, void * context){
	printf("PID:%s\n", arguments[0]);
	return 0;
}


/**
 * Prints the program output that the kernel has sent so far (programOutput route)
 * @param arguments [pid]
 */
int printProgramOutput(char ** arguments, void * context){
    // TODO: Complete
    return 0;
}








