#include <stdlib.h>
#include "kernel.h"
#include "../easysocket/socketset.h"
#include "../easysocket/utils.h"


/**
 * Changes the degree of multiprogramming of the kernel
 * It takes a string because the value will come from either user input or config
 * @param kernel
 * @param multiprogDegree
 */
void changeMultiprogDegree(t_kernel * kernel, char * multiprogDegree) {
	kernel->multiprogDegree = atoi(multiprogDegree);
}

/**
 * Initialize the kernel and all its properties
 * @return the init'd kernel
 */
t_kernel * initKernel(char * multiprogDegree, t_log_level logLevel) {
	// Alloc space
	t_kernel * kernel = malloc(sizeof(t_kernel));

	// Init the logger
	kernel->log = log_create(DEFAULT_LOG_OUTPUT_FILE, KERNEL_NAME, true, logLevel);

	// Initialize queues & list
	kernel->processList = list_create();
	int i;
	for(i = 0; i < PROCESS_STATES_COUNT; i++){
		kernel->processQueues[i] = queue_create();
	}

	// Start the multiprogramming degree & start pidCounter, runningProcesses
	changeMultiprogDegree(kernel, multiprogDegree);
	kernel->pidCounter = 0;
	kernel->runningProcesses = 0;

	// Init the planning mutex
	kernel->planningMutex = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(kernel->planningMutex, NULL);

	// Init socketSet (but don't connect to anything yet)
	kernel->socketSet = newSocketSet();

	// Return init'd kernel
	return kernel;
}


/**
 * Goes through the queues, checking if a process needs/can be moved onto its next step
 * @param kernel
 */
//void scheduleQueues(t_kernel * kernel) {
//	pthread_mutex_lock(kernel->planningMutex);
//	unsafeScheduleQueues(kernel);
//	pthread_mutex_unlock(kernel->planningMutex);
//}


/**
 * Goes through the queues, checking if a process needs/can be moved onto its next step
 * It's unsafe because it assumes that the kernel planning mutex has been locked
 * @param kernel
 */
//void unsafeScheduleQueues(t_kernel * kernel) {
//	// First check if there are procs in the NEW that can be moved to READY
//	// This will happen if the deg of multiprogramming allows it
//	queue_size(kernel->processQueues[])
//}


/**
 * Sets the process' state and adds it to the queue
 * @param kernel
 * @param queue
 * @param pcb
 */
int addProcessToQueue(t_kernel * kernel, programState queue, t_pcb * pcb) {
	// Lock the planning mutex to make the change
	pthread_mutex_lock(kernel->planningMutex);

	switch(queue) {
		case NEW:
			// Assign the PID & add it to the global process list
			pcb->PID = kernel->pidCounter++;
			list_add(kernel->processList, pcb);

			// Check if the process can be sent straight to the READY queue
			if(kernel->multiprogDegree > kernel->runningProcesses){
				log_debug(kernel->log, "Multiprog. deg. %i > %i running procs ", kernel->multiprogDegree, kernel->runningProcesses);
				queue = READY; // Yes it can
			}

		case READY:
			// If the kernel has reached it's max concurrent program number, return error
			if(kernel->multiprogDegree <= kernel->runningProcesses){
				pthread_mutex_unlock(kernel->planningMutex);
				return -1;
			}

			// We can add the process to ready
			log_debug(kernel->log, "Increasing runningProcesses counter");
			kernel->runningProcesses++;
			break;

		case EXIT:
			break;
		case EXEC:
			break;
		case BLOCK:
			break;
	}


	// Update the resulting state for the process and push it to the queue
	pcb->state = queue;
	queue_push(kernel->processQueues[queue], pcb);
	log_info(kernel->log, "Added process PID:%i to queue %s", pcb->PID, programStateString[queue]);

	// Now that we've updated the queues, check the state of the whole thing and
	// see if a process can be moved from their queue
//	unsafeScheduleQueues(kernel);

	// Unlock the mutex and that's it
	pthread_mutex_unlock(kernel->planningMutex);
	return 0;
}