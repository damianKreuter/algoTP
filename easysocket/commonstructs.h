#ifndef SIMPLE_EXAMPLE_HANDLERARGUMENTS_H
#define SIMPLE_EXAMPLE_HANDLERARGUMENTS_H

#include <pthread.h>
#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <commons/log.h>

/**
 * These structs are all defined here to avoid problems with circular definitions in structures
 * E.g. handler_arguments contains a t_socketSet, t_socketDescriptor contains a handler_arguments
 */


/**
 * Define types based on the socket structs to avoid compiler warnings
 */

typedef struct sockaddr t_sockaddr;
typedef struct addrinfo t_addrinfo;


/** Just a t_dictionary wrapper to avoid using those functions directly
 *(The only way to utilize the socketset should be with this lib,
 * and not with the commons dictionary api)
 */
typedef struct socketset {
    pthread_mutex_t * lockMutex;
    t_dictionary * sockets;
} t_socketSet;


/**
 * Custom structs
 * Note: the reason why we pass the socketSet and the socketName instead of the whole socketDescriptor is
 * to force the use of the thread-safe methods in this lib
 */
struct socketContext {
    int socketFD;
    t_dictionary * routes;
    char * currentSocketName;
    t_socketSet * socketSet;
	t_log * log;
    pthread_mutex_t * shouldTerminate;
	void * processContext; // Ugly, but the only way to do it without global vars (ew)
};

typedef struct socketContext t_socketContext;


/**
 * Enum for the different types of socket (basically 2)
 */
typedef enum {CLIENT, SERVER, SERVER_CONNECTION} socketType;


/**
 * Descriptor for a socket. Contains its file descriptor, the type (client/server)
 * and if it is a server, an array of current connections
 */
typedef struct socketDescriptor {
    int socketFD;
    socketType type;
    pthread_t handlerThread;
    t_socketContext * socketContext;
    t_list * connections;
} t_socketDescriptor;

#endif //SIMPLE_EXAMPLE_HANDLERARGUMENTS_H
