#ifndef SIMPLE_EXAMPLE_KERNEL_H
#define SIMPLE_EXAMPLE_KERNEL_H

#include <commons/collections/queue.h>
#include "../easysocket/pcb.h"
#include "../easysocket/commonstructs.h"

#define KERNEL_NAME "kernel"

#define PS_PID_COLUMN_WIDTH 10
#define PS_STATE_COLUMN_WIDTH 15

typedef struct kernel {
	/**
	 * The processList will hold the same references as the processQueues
	 * Sharing them like this allows easier traversal of both structures, an easy
	 * way of assigning new PIDs, etc.
	 */
	t_list * processList;
	int pidCounter;
	int runningProcesses;
	t_queue * processQueues[PROCESS_STATES_COUNT];
	t_socketSet * socketSet;
	pthread_mutex_t * planningMutex;
	int multiprogDegree;
	t_log * log;
} t_kernel;


t_kernel * initKernel(char * multiprogDegree, t_log_level logLevel);
void changeMultiprogDegree(t_kernel * kernel, char * multiprogDegree);
int addProcessToQueue(t_kernel * kernel, programState queue, t_pcb * pcb);

#endif //SIMPLE_EXAMPLE_KERNEL_H
