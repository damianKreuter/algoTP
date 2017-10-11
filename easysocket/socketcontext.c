#include <stdlib.h>
#include <commons/log.h>
#include <commons/string.h>
#include <string.h>
#include "commonstructs.h"
#include "socketcontext.h"
#include "socketset.h"

#define LOG_PRINT_TO_CONSOLE true

/**
 * Alloc's a new handler_arguments struct
 * @return the pointer to the new struct
 */
t_socketContext * newSocketContext(char * socketName, t_log_level logLevel) {
	t_socketContext * socketContext = malloc(sizeof(t_socketContext));

	// Create a mutex to have a nice way to let threads know if they should terminate
	socketContext->shouldTerminate = malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(socketContext->shouldTerminate, NULL);
	// Start it locked so that the threads can trylock() and see that
	pthread_mutex_lock(socketContext->shouldTerminate);
	socketContext->currentSocketName = socketName;
	char * logName =string_duplicate(socketName);
	string_append(&logName, "_socket");
	socketContext->log = log_create(DEFAULT_LOG_OUTPUT_FILE, logName, LOG_PRINT_TO_CONSOLE, logLevel);
	free(logName);
	return socketContext;
}


/**
 * Checks the provided socketContext' mutex to see if it can be locked
 * If so it immediately unlocks and returns 1, otherwise it returns 0
 * @param socketContext
 * @return
 */
bool shouldHandlerThreadTerminate(t_socketContext * socketContext) {
	if(pthread_mutex_trylock(socketContext->shouldTerminate) == 0) {
		pthread_mutex_unlock(socketContext->shouldTerminate);
		return true;
	} else {
		return false;
	}
}


/**
 * Copies a socketContext, changing the socketFD (used for server connections)
 * @param socketContext
 * @param newSocketFD
 * @return
 */
t_socketContext * copySocketContext(t_socketContext * socketContext, int newSocketFD) {
	t_socketContext * socketContextCopy = newSocketContext(socketContext->currentSocketName, socketContext->log->detail);
	socketContextCopy->socketFD = newSocketFD;

	// Copy the references that will remain the same from the provided context
	// We need to lock to ensure they aren't being written in the meantime
	lockSocketSet(socketContext->socketSet);
	socketContextCopy->processContext    = socketContext->processContext;
	socketContextCopy->socketSet         = socketContext->socketSet;
	socketContextCopy->routes            = socketContext->routes;
	unlockSocketSet(socketContext->socketSet);

	return socketContextCopy;
}