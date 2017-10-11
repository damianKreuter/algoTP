/** This file contains the command handlers that can be executed from the main process **/


/*
 * Commands
 */


#include <commons/string.h>
#include <string.h>
#include "kernel.h"

/**
 * Lists all the processes in all queues or one if specified
 * @param arguments [queue]
 */
int listProcesses(char ** arguments, void * context){
	t_kernel * kernel = context;
	char * queueArg = arguments[0];
	int i, queue = -1;

	// Check if a queue was specified and turn it to a state
	if(queueArg != NULL){
		string_to_upper(queueArg);
		for(i = 0; i < PROCESS_STATES_COUNT; i++){
			if(!strcmp(programStateString[i], queueArg)){
				queue = i;
				break;
			}
		}
		if(queue == -1){
			log_warning(kernel->log, "The queue provided does not exist");
			return 0;
		}else{
			log_debug(kernel->log, "Showing processes for queue %i", queue);
		}
	}

	// Save output then log all at once
	char * output = string_new();
	string_append_with_format(&output, "Process info:\n\t%-*s%-*s", PS_PID_COLUMN_WIDTH,   "PID",
	                                                                PS_STATE_COLUMN_WIDTH, "STATE");

	// Define the function in context
	void printProcess(void * pcbRef) {
		t_pcb * pcb = pcbRef;
		printf("%i", pcb->PID);
		printf("%i", pcb->state);
		printf("%s", programStateString[pcb->state]);
		log_debug(kernel->log, "\n %i:%i:%s; ", pcb->PID, pcb->state, programStateString[pcb->state]);
		if(queue == -1 || queue == pcb->state)
			string_append_with_format(&output, "\n\t%-*i%-*s", PS_PID_COLUMN_WIDTH,   pcb->PID,
				                                               PS_STATE_COLUMN_WIDTH, programStateString[pcb->state]);
	}

	// Add separator and iterate all queues (locking to keep things still)
	pthread_mutex_lock(kernel->planningMutex);
	list_iterate(kernel->processList, printProcess);
	pthread_mutex_unlock(kernel->planningMutex);
	string_append(&output, "\n");

	// Finally log it all
	log_info(kernel->log, output);

	return 0;
}


/**
 * Prints all the stats for a given process (PID)
 * @param arguments [PID]
 */
int processStats(char ** arguments, void * context){
	// TODO: Complete
	return 0;
}


/**
 * Prints the whole Global File Table
 * @param arguments []
 */
int printGlobalFileTable(char ** arguments, void * context){
	// TODO: Complete
	return 0;
}


/**
 * Changes the degree of multiprogramming to the specified value
 * @param arguments [degree]
 */
int changeMultiprogrammingDeg(char ** arguments, void * context){
	// TODO: Complete
	return 0;
}


/**
 * Kills the specified running process
 * @param arguments [PID]
 */
int killProcess(char ** arguments, void * context){
	// TODO: Complete
	return 0;
}


/**
 * Pauses the process scheduling (no state change until resumed)
 * @param arguments []
 */
int pauseScheduling(char ** arguments, void * context){
	// TODO: Complete
	return 0;
}


/**
 * Resumes the process scheduling
 * @param arguments [queue]
 */
int resumeScheduling(char ** arguments, void * context){
	// TODO: Complete
	return 0;
}

