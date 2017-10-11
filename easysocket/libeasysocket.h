//
// Created by Alejo on 3/29/2017.
//

#ifndef SIMPLE_EXAMPLE_SOCKET_H
#define SIMPLE_EXAMPLE_SOCKET_H

#include <commons/collections/dictionary.h>
#include "payload.h"
#include "socketset.h"
#include "socketcontext.h"
#include "commonstructs.h"

#define BACKLOG 10            // How many pending connections the queue will hold
#define MAXDATASIZE 100       // Max size of the buffer to read (in bytes)
#define LOCALHOST "127.0.0.1" // Just a shortcut
#define DEFAULT_LOG_OUTPUT_FILE "out.txt"

extern t_log * socklog;


// Aux struct
typedef struct connectionListenerArgs {
	char * socketName;
	t_socketSet * socketSet;
	t_socketContext * context;
	void * (*handler)(void*);
} t_connectionListenerArgs;


/**
 * Struct to hold the route handler for a socket
 */
typedef struct route {
	char * endpoint;
	// The handler should take (ARGS, CONTEXT)
	int (*handler)(char **, t_socketContext *);
	char * docs;
} t_route;


/**
 * Struct to make a sort of builder pattern, the socket create funcs have way too many args
 */
typedef struct socketConfig {
	char * port;
	char * address;
	char * name;
	t_route * routes;
	t_log_level logLevel;
} t_socketConfig;


/**
 * Function declarations
 */

void *defaultConnectionHandler(void * args);
int sendMessagesToSocket(t_socketSet * socketSet, char * socketName, char *endpoint, char ** arguments);
int sendMessagesInContext(t_socketContext * socketContext, char * endpoint, char ** arguments);
int sendMessageInContext(t_socketContext * socketContext, char * endpoint, char * message);
int createSocket(t_addrinfo *addressInfo);
//int createServerSocketWithHandler(char* port, int backlogQueueAmount, t_dictionary *routes, void* (*connectionHandler)(void*), char * socketName, t_socketSet * socketSet, t_log_level  logLevel);
int createServerSocket(t_socketConfig * socketConfig, t_socketSet * socketSet, void * processContext);
int createClientSocket(t_socketConfig * config, t_socketSet * socketSet, void * processContext);
int sendMessageToSocket(t_socketSet *socketSet, char *socketName, char *endpoint, char * message);

#endif //SIMPLE_EXAMPLE_SOCKET_H
