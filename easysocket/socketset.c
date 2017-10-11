//
// Created by Alejo on 5/6/2017.
//

#include <commons/collections/list.h>
#include <commons/collections/dictionary.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "socketset.h"
#include "utils.h"


/**
 * Receives one of the socketType enum and returns the string
 * @param type
 * @return
 */
char * getSocketTypeString(socketType type){
    return type? "SERVER" : "CLIENT";
}


/**
 * Dynamically creates a new socket set
 * @return the new socketSet's pointer
 */
t_socketSet * newSocketSet() {
    t_socketSet * socketSet = malloc(sizeof(t_socketSet));
    socketSet->lockMutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(socketSet->lockMutex, NULL);
    socketSet->sockets = dictionary_create();
    return socketSet;
}


/**
 * Locks the provided socketSet (actually locks the mutex within)
 * @param socketSet
 */
void lockSocketSet(t_socketSet * socketSet) {
    pthread_mutex_lock(socketSet->lockMutex);
}


/**
 * Unlocks the provided socketSet (actually unlocks the mutex within)
 * @param socketSet
 */
void unlockSocketSet(t_socketSet * socketSet) {
    pthread_mutex_unlock(socketSet->lockMutex);
}


/**
 * Destroys a socketSet.
 * Note: Does not clean the dictionary elements (for that call waitOn[..] functions)
 * @param socketSet
 */
void destroySocketSet(t_socketSet * socketSet) {
    pthread_mutex_destroy(socketSet->lockMutex);
    free(socketSet->lockMutex);
    dictionary_destroy(socketSet->sockets);
    free(socketSet);
}


/**
 *  Dynamically creates a new socketDescriptor struct)
 *  @return the new socketDescriptor's pointer
 */
t_socketDescriptor * newSocketDescriptor(int socketFD, socketType type, pthread_t handlerThread, t_list * connections, t_socketContext * socketContext) {
    t_socketDescriptor * socketDescriptor = malloc(sizeof(t_socketDescriptor));
    socketDescriptor->socketFD = socketFD;
    socketDescriptor->type = type;
    socketDescriptor->handlerThread = handlerThread;
    socketDescriptor->connections = connections;
    socketDescriptor->socketContext = socketContext;
    return socketDescriptor;
}


/**
 * Destroys a socketDescriptor (as long as it does not contain open connections)
 * @param socketDescriptor
 * @return 0 for success, -1 for error
 */
int destroySocketDescriptor(t_socketDescriptor * socketDescriptor) {
    if(socketDescriptor->connections != NULL){
        if(list_size(socketDescriptor->connections) > 0){
            logContext(socketDescriptor->socketContext, error, "The socketDescriptor cannot be destroyed, there are open connections remaining");
            return -1;
        }else{
            free(socketDescriptor->connections);
        }
    }

    // We can write these because we know the thread has already finished and
    // the socketContext are a per thread struct
    pthread_mutex_destroy(socketDescriptor->socketContext->shouldTerminate);
	log_destroy(socketDescriptor->socketContext->log);
    free(socketDescriptor->socketContext->shouldTerminate);
	dictionary_destroy(socketDescriptor->socketContext->routes);
    free(socketDescriptor->socketContext);
    free(socketDescriptor);
    return 0;
}


/**
 * Adds a socketDescriptor to the socket dictionary
 * (Thread safe)
 *
 * @param socketSet
 * @param name
 * @param socketDescriptor
 */
void addSocketDescriptorToSet(t_socketSet * socketSet, char * name, t_socketDescriptor * socketDescriptor){
    lockSocketSet(socketSet);
    dictionary_put(socketSet->sockets, name, socketDescriptor);
    unlockSocketSet(socketSet);
}

/**
 * Creates a new socketDescriptor from the provided params and adds it to the socketSet
 *
 * @param socketSet
 * @param name
 * @param socketFD
 * @param type
 * @param connections
 * @return the address in memory of the socketDescriptor thread (to use if the set needs
 * to be added to the set before the thread is up)
 *
 */
void addSocketToSet(t_socketSet * socketSet, char * name, int socketFD, socketType type, pthread_t handlerThread, t_list * connections, t_socketContext * socketContext){
	log_debug(socklog, "Adding socket '%s' to set", name);
    t_socketDescriptor * socketDescriptor = newSocketDescriptor(socketFD, type, handlerThread, connections, socketContext);
    addSocketDescriptorToSet(socketSet, name, socketDescriptor);
}


/**
 * Adds a socketDescriptor to the socket connections list
 * (Thread safe)
 *
 * @param socketSet
 * @param name
 * @param socketDescriptor
 */
void addConnectionDescriptorToSet(t_socketSet * socketSet, char * name, t_socketDescriptor * socketDescriptor){
	lockSocketSet(socketSet);
	t_socketDescriptor * serverDescriptor = (t_socketDescriptor *) dictionary_get(socketSet->sockets, name);
	list_add(serverDescriptor->connections, socketDescriptor);
	unlockSocketSet(socketSet);
}


/**
 * Creates a new socketDescriptor from the provided params and adds it to the list of connections of the socket
 *
 * @param socketSet
 * @param name
 * @param socketFD
 * @param type
 * @param connections
 */
void addConnectionToSocket(t_socketSet * socketSet, char * name, int socketFD, pthread_t handlerThread, t_socketContext * socketContext){
    t_socketDescriptor * socketDescriptor = newSocketDescriptor(socketFD, SERVER_CONNECTION, handlerThread, NULL, socketContext);
    addConnectionDescriptorToSet(socketSet, name, socketDescriptor);
}


/**
 * Pretty prints to console the provided socketDescriptor
 * @param socketDescriptor
 */
void printSocketDescriptor(t_socketDescriptor * socketDescriptor) {
    int socketConnections = (socketDescriptor->connections == NULL)? 0 : list_size(socketDescriptor->connections);

	log_debug(socklog, "\tSocket FD: %i", socketDescriptor->socketFD);
	log_debug(socklog, "\tSocket type: %s", getSocketTypeString(socketDescriptor->type));
    // The line below is kinda useless, the thread id is pretty large but whatever
	log_debug(socklog, "\tSocket handler thread FD: %lu", (unsigned long) socketDescriptor->handlerThread);
	log_debug(socklog, "\tSocket number of connections: %i", socketConnections);
	if(socketConnections > 0)
		list_iterate(socketDescriptor->connections, (void (*)(void*)) printSocketDescriptor);
}


/**
 * Function to pass to a dictionary_iterator()
 * Receives a socketdescriptor and prints all necessary information to console
 * @param socketSet
 */
void printSocketDescriptorIteratorFunction(char * socketName, void * value){
    t_socketDescriptor * socketDescriptor = (t_socketDescriptor *) value;
	log_debug(socklog, "Socket '%s'", socketName);
    printSocketDescriptor(socketDescriptor);
}


/**
 * Receives a socketSet and prints all necessary information to console
 * @param socketSet
 */
void printSocketSet(t_socketSet * socketSet) {
    lockSocketSet(socketSet);
    dictionary_iterator(socketSet->sockets, printSocketDescriptorIteratorFunction);
    unlockSocketSet(socketSet);
}


/**
 * Retreives the socketFD for the socket from the set with the provided socketName
 * @param socketSet
 * @param socketName
 * @return the socketFD requested
 */
int getSocketFD(t_socketSet * socketSet, char * socketName) {
    lockSocketSet(socketSet);
    t_socketDescriptor * socketDescriptor = dictionary_get(socketSet->sockets, socketName);
    int socketFD = socketDescriptor->socketFD;
    unlockSocketSet(socketSet);
    return socketFD;
}


/**
 * Retreives the socketFD for the socket from the provided context
 * Note: the locking is necessary in case the var is being written while this executes
 * Well I'm actually not sure since I'm getting the set ref in the same way but my head hurts
 * @param socketSet
 * @param socketName
 * @return the socketFD requested
 */
int getSocketFDFromContext(t_socketContext * socketContext) {
	t_socketSet * socketSet = socketContext->socketSet;
	lockSocketSet(socketSet);
	int socketFD = socketContext->socketFD;
	unlockSocketSet(socketSet);
	return socketFD;
}



/**
 * Set the handler thread for the socket after the fact (used in createServerSocket)
 * @param socketSet
 * @param socketName
 * @param thread
 */
void setSocketHandlerThread(t_socketSet * socketSet, char * socketName, pthread_t thread){
	lockSocketSet(socketSet);
	t_socketDescriptor * socketDescriptor = dictionary_get(socketSet->sockets, socketName);
	socketDescriptor->handlerThread = thread;
	unlockSocketSet(socketSet);
}


// Circular ref between this and the func below, predeclare here
void waitAndCloseAndDestroySocketDescriptor(void * socketDescriptorRef, t_socketSet * socketSet, bool force);

/**
 * Waits with join for thread to terminate (should be JOINABLE) and closes the socket
 * @param thread
 * @param socketFD
 */
void waitOnHandlerThreadAndCloseSocket(pthread_t thread, int socketFD, pthread_mutex_t *shouldTerminate,
                                       socketType type, t_list *connections, t_socketSet * socketSet) {
    int threadReturnValue;

    // If the shouldTerminate mutex is passed then unlock it so threads can lock
    if(shouldTerminate != NULL){
	    log_trace(socklog, "Unlocking mutex to notify that thread should terminate");
	    pthread_mutex_unlock(shouldTerminate);
    }

	log_debug(socklog, "Waiting for socket handler thread %lu to terminate...", (unsigned long) thread);
    pthread_join(thread, (void *) &threadReturnValue);
	log_debug(socklog, "Handler thread %lu joined successfully with return value: %i", (unsigned long) thread, threadReturnValue);


	if(type == SERVER && connections != NULL && list_size(connections) > 0){
		bindContext(waitAndCloseAndDestroySocketDescriptor, iteratorFunction, socketDescriptorRef, socketSet, shouldTerminate != NULL);
		log_debug(socklog, "The socket is type SERVER and has remaining connections, terminating those...");

		// Since waitAndCloseAndDestroySocketDescriptor unlocks then locks, we must accommodate for that
		lockSocketSet(socketSet);
		list_clean_and_destroy_elements(connections, iteratorFunction);
		unlockSocketSet(socketSet);
		log_debug(socklog, "All connections terminated");
	}

    close(socketFD);
	log_debug(socklog, "Socket %i closed successfully", socketFD);
}


/**
 * Waits for the handler thread to terminate, closes the socket and destroys the descriptor
 * Has to be declared nested to access the socketSet var
 */
void waitAndCloseAndDestroySocketDescriptor(void * socketDescriptorRef, t_socketSet * socketSet, bool force){
	// Cast the void pointer to the correct struct and extract the needed data
	t_socketDescriptor * socketDescriptor = (t_socketDescriptor *) socketDescriptorRef;
	pthread_t thread                  = socketDescriptor->handlerThread;
	pthread_mutex_t * shouldTerminate = socketDescriptor->socketContext->shouldTerminate;
	int socketFD                      = socketDescriptor->socketFD;
	t_list * connections              = socketDescriptor->connections;
	socketType type                   = socketDescriptor->type;

	// Unlock the set while waiting to avoid deadlocks.
	// IMPORTANT: this is potentially dangerous: this socketDescriptor could be changed while waiting on the thread
	// TODO: Maybe because of the above, add the option to make the socket listener in the main thread
	log_debug(socklog, "Waiting for termination of socket descriptor (FD: %i); starting with terminating the thread, unlocking set to wait", socketFD);
	unlockSocketSet(socketSet);
	waitOnHandlerThreadAndCloseSocket(thread, socketFD, force ? shouldTerminate : NULL, type, connections, socketSet);
	log_debug(socklog, "Thread terminated and socket closed, locking set and destroying the socketDescriptor ref");
	lockSocketSet(socketSet);

	// Destroy the socketDescriptor and that's it
	destroySocketDescriptor(socketDescriptor);
};


/**
 * Wait for all socket threads to terminate and then close them all (freeing all vars)
 * @param socketSet
 */
void waitAndCloseAllSocketsAndDestroySet(t_socketSet * socketSet, bool force) {
	lockSocketSet(socketSet);
	log_warning(socklog, "Started termination flow, %s", force? "forcing termination" : "waiting for things to finish on their own");

	// Since the iterator only passes the ref, bind context to do what we need
	bindContext(waitAndCloseAndDestroySocketDescriptor, iteratorFunction, socketDescriptorRef, socketSet, force);

    // Destroy all socketDescriptors with the function declared (nested) above
    dictionary_clean_and_destroy_elements(socketSet->sockets, iteratorFunction);
	log_info(socklog, "Destroyed all descriptors; unlocking and destroying set");

    // Unlock the set and free everything
    unlockSocketSet(socketSet);
    destroySocketSet(socketSet);
	log_info(socklog, "All destroyed");
}


/**
 * Checks that there is a socket in the set with the provided name
 * @param socketSet
 * @param socketName
 * @return
 */
bool setHasSocket(t_socketSet * socketSet, char * socketName) {
	lockSocketSet(socketSet);
	bool result = dictionary_has_key(socketSet->sockets, socketName);
	unlockSocketSet(socketSet);
	return result;
}

