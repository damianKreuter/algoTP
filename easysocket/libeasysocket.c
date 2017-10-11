/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <pthread.h>

#include <commons/string.h>
#include <commons/log.h>
#include <commons/collections/dictionary.h>
#include <errno.h>

#include "payload.h"
#include "socketset.h"
#include "libeasysocket.h"
#include "utils.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-noreturn"

t_log * socklog = NULL;


/**
 * Instances the global log variable if it's not already set
 */
void initLog(t_log_level logLevel) {
	if(socklog == NULL) socklog = log_create(DEFAULT_LOG_OUTPUT_FILE, "socketlib", true, logLevel);
}


/**
 * Gets the socket address from a sockaddr struct, either IPv4 or IPv6
 */
void * get_in_addr(t_sockaddr *socketAddress) {
    if (socketAddress->sa_family == AF_INET) {
        return &(((struct sockaddr_in*) socketAddress)->sin_addr);
    }

    return &(((struct sockaddr_in6*) socketAddress)->sin6_addr);
}


/**
 * Returns the info for the local address
 * @param serverInfo
 * @return
 */
int getServerAddrInfo(char * serverAddress, char * port, t_addrinfo ** serverInfo){
    // Structure to hold the hints to pass to getaddrinfo()
    t_addrinfo addrInfoHints;

    // First make sure the struct is empty (initialize) by setting every byte to 0
    memset(&addrInfoHints, 0, sizeof addrInfoHints);

    // Add the hints to pass to getaddrinfo()
    addrInfoHints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
    addrInfoHints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    addrInfoHints.ai_flags = AI_PASSIVE;     // fill in my IP for me

    // Get the info and return
    return getaddrinfo(serverAddress, port, &addrInfoHints, serverInfo);
}


/**
 * Makes a routes dictionary (t_dictionary*) from a t_route array
 * @param routes
 * @param length
 * @return
 */
t_dictionary * newRoutesDictionary(t_route * routes) {
	int i;
	t_dictionary * routesDictionary = dictionary_create();
	for(i = 0; routes[i].endpoint != NULL; i++){
		dictionary_put(routesDictionary, routes[i].endpoint, routes[i].handler);
	}
	return routesDictionary;
}


/**
 * Create a socket with the data from a addrinfo struct
 * @param addressInfo
 * @return
 */
int createSocket(t_addrinfo *addressInfo) {
    return socket(addressInfo->ai_family, addressInfo->ai_socktype, addressInfo->ai_protocol);
}


/**
 * Function to pass to createSocketAndDoFunc. Sets the socket options
 * and attempts to bind it to the address provided
 * @param socketFD
 * @param addrInfo
 * @return -1 if unsuccessful, 0 if successful
 */
int bindServerSocket(int socketFD, t_addrinfo *addrInfo) {
    // I dunno what this does :P
    int yes = 1;

    // Set the socket options
    if (setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        // If it returns -1 there was an error, log and just exit
        log_error(socklog, "Error while setting socket options");
        exit(1);
    }

    // Bind the socket
    if (bind(socketFD, addrInfo->ai_addr, addrInfo->ai_addrlen) == -1) {
        // If it returns -1 there was an error, close the socket and try with the next address
        close(socketFD);
        log_error(socklog, "Could not bind socket to current address"); //TODO: print the referred address
        return -1;
    }

    // Successfully bound the socket
    return 0;
}


/**
 * Function to pass to createSocketAndDoFunc. Attempts to connect the
 * socket to the address provided
 * @param socketFD
 * @param addrInfo
 * @return -1 if unsuccessful, 0 if successful
 */
int connectClientSocket(int socketFD, t_addrinfo *addrInfo) {
    char address[INET6_ADDRSTRLEN];

    if (connect(socketFD, addrInfo->ai_addr, addrInfo->ai_addrlen) == -1) {
        close(socketFD);
        log_error(socklog, "client: connect");
        return -1;
    }

    // Convert the connecting address to readable string and print to console
    inet_ntop(addrInfo->ai_family,
              get_in_addr(addrInfo->ai_addr),
              address, sizeof address);
    log_info(socklog, "Connecting to %s", address);

    // Socket successfully connected
    return 0;
}


/**
 * Create the socket and perform the passed function.
 * @param address
 * @param port
 * @param serverInfo
 * @param function
 * @return
 */
int createSocketAndDoFunc(char *address, char *port, t_addrinfo *serverInfo, int (*function)(int, t_addrinfo*)) {
    int getaddrinfoErrorValue, socketFD = -1;
    t_addrinfo *currentAddrInfo;

	// Max time recv() can hang
	struct timeval timeVal;
	timeVal.tv_sec = 1;
	timeVal.tv_usec= 0;

    // Get the server address info
    if ((getaddrinfoErrorValue = getServerAddrInfo(address, port, &serverInfo)) != 0) {
        // If getaddrinfo() returns 0 then there was an error
        // Print error to console and return
        log_error(socklog, "getaddrinfo: %s", gai_strerror(getaddrinfoErrorValue));
        return -1;
    }

    // Loop through all the info results and bind to the first one we can
    for(currentAddrInfo = serverInfo; currentAddrInfo != NULL; currentAddrInfo = currentAddrInfo->ai_next) {
        // Try to create the socket with the current address info
        if ((socketFD = createSocket(currentAddrInfo)) == -1) {
            // If the file descriptor is -1 then there was an error opening the socket, log and continue
	        log_error(socklog, "There was a problem creating the socket");
            continue;
        }

	    // Set a timeout for recv() so the listener thread can check if it should terminate
	    if (setsockopt(socketFD, SOL_SOCKET, SO_RCVTIMEO, (const char *) &timeVal, sizeof(struct timeval)) == -1) {
		    // If it returns -1 there was an error, log and just exit
		    log_error(socklog, "Error while setting socket options");
		    exit(1);
	    }

        // Execute the function. If it returns -1 continue the loop.
        if(((*function)(socketFD, currentAddrInfo)) == -1)
            continue;

        // If this point is reached then a socket was successfully created and bound
        break;
    }

    // We no longer need the server address info structure, and we can free the memory
    freeaddrinfo(serverInfo);

    // Check the last address info, to see if the loop ended without success
    if (currentAddrInfo == NULL)  {
        // If the current address info is null then it means no socket was created and bound, log and exit
	    log_error(socklog, "server: failed to bind");
        exit(1);
    }

    // Socket was successfully created and the function executed
    return socketFD;
}


/**
 * Creates a new thread and starts listening for incoming messages on the socket
 */
pthread_t createThread(void * args, void *(*threadHandler)(void *), int detachState) {
	// Thread variables
	pthread_t thread;                // Reference to the thread to be created
	pthread_attr_t threadAttributes; // Thread attributes

	// Initialize the thread attributes and set them to DETACHED
	// (that way when the thread ends it frees its resources and main doesn't wait for them to end/join)
	pthread_attr_init(&threadAttributes);
	pthread_attr_setdetachstate(&threadAttributes, detachState);

	// Create a new thread to respond to the client in parallel to waiting for a new connection
	log_debug(socklog, "Spinning up a new thread...");
	pthread_create(&thread, &threadAttributes, threadHandler, args);
	return thread;
}


/**
 * Creates a new thread and starts listening for incoming messages on the socket
 */
pthread_t createThreadWithContext(t_socketContext *socketContext, void *(*threadHandler)(void *), int detachState) {
    return createThread((void *) socketContext, threadHandler, detachState);
}


/**
 * Wait for incoming connections on the server
 * Declared here to access connectionHandler, socketName & socketSet
 */
void * listenForConnections(void * args) {
	// Cast
	t_connectionListenerArgs * listenerArgs = (t_connectionListenerArgs *) args;

	// Var to save the new connection FD
	int newConnectionFD;
	bool connectionFound = true;

	// Var to hold the plain address of an incoming connection
	char address[INET6_ADDRSTRLEN];

	// Connector address address information
	struct sockaddr_storage connectingAddress;

	// Var to hold the connecting address size
	socklen_t connectingAddressSize;

	while(true) {
		// Check the socketContext to see if the thread should terminate
		if(shouldHandlerThreadTerminate(listenerArgs->context))
			break;

		// Print waiting message to console, now with spinner!
		if(connectionFound){
			// Will auto flush due to the \n
			logContext(listenerArgs->context, info, "Waiting for client connection... ");
			fflush(stdout);
			connectionFound = false;
		}

		// Get the size of the connecting address struct
		connectingAddressSize = sizeof connectingAddress;

		// Wait for a new connection
		newConnectionFD = accept(listenerArgs->context->socketFD, (struct sockaddr *) &connectingAddress, &connectingAddressSize);
		if (newConnectionFD == -1) {
			if(errno != EWOULDBLOCK && errno != EAGAIN)
				logContext(listenerArgs->context, error, "There was an error while waiting for client connections:");
//			printf("\b%c", consoleSpinner());
//			fflush(stdout);
			continue;
		}else{
			connectionFound = true;
		}

		// Convert the connecting address to readable string and print to console
		inet_ntop(connectingAddress.ss_family,
		          get_in_addr((struct sockaddr *)&connectingAddress),
		          address, sizeof address);
		logContext(listenerArgs->context, info, "Server: got connection from %s, assigning FD %i", address, newConnectionFD);

		// Copy the context from the server socket, and change the FD to the connection's
		t_socketContext * connectionSocketContext = copySocketContext(listenerArgs->context, newConnectionFD);

		// Start a new thread to handle the new connection and go back to accepting new ones
		pthread_t connectionThread = createThreadWithContext(connectionSocketContext, listenerArgs->handler, PTHREAD_CREATE_JOINABLE);

		// If the user provided a name and a socketSet, make the descriptor and add it to the set
		if(listenerArgs->socketName != NULL && listenerArgs->socketSet != NULL){
			addConnectionToSocket(listenerArgs->socketSet, listenerArgs->socketName, newConnectionFD, connectionThread, connectionSocketContext);
		}
	}

	// Free what needs to be freed
	free(listenerArgs);

	// If we exited the loop then the thread should end
	pthread_exit(NULL);

	// TODO: Return something useful
	return NULL;
}


/**
 * Create a socket, bind it and set it to listen for incoming connections
 * When a new client connects to it, it will spin up a new thread passing
 * it the provided handler function
 * @param connectionHandler the function to execute on a new connection
 * @return
 */
int createServerSocketWithHandler(char* port, int backlogQueueAmount, t_dictionary * routes, void * (*connectionHandler)(void*), char * socketName, t_socketSet * socketSet, t_log_level logLevel, void * processContext) {
	log_debug(socklog, "Creating server socket '%s'", socketName);
    // We will listen on socketFD
    int socketFD;

    // Data to pass to getaddrinfo on addrinfoHints, address result on serverInfo
    t_addrinfo * serverInfo = NULL;

    // Create the socket
    socketFD = createSocketAndDoFunc(LOCALHOST, port, serverInfo, bindServerSocket);

	// Thread that will accept connections
	pthread_t thread = 0;

	// List of connections for this socket
	t_list * connections = list_create();

    // Start listening for incoming socket connections
    if (listen(socketFD, backlogQueueAmount) == -1) {
        // There was an error starting to listen, log and exit
        log_error(socklog, "There was an error starting to listen().");
        exit(1);
    }

	// We have to do it like this (allocating memory manually) because if we declare it statically then
	// we would pass the pointer to the new thread and once we exit this function since we exit the scope
	// of the variable it would get free'd and we need it elsewhere
	t_socketContext * socketContext = newSocketContext(socketName, logLevel);
	socketContext->routes = routes;
	socketContext->currentSocketName = socketName;
	socketContext->socketSet = socketSet;
	socketContext->socketFD = socketFD;
	socketContext->processContext = processContext;

	t_connectionListenerArgs * connectionArgs = malloc(sizeof(t_connectionListenerArgs));
	connectionArgs->handler = connectionHandler;
	connectionArgs->context = socketContext;
	connectionArgs->socketName = socketName;
	connectionArgs->socketSet = socketSet;

	// If the user provided a name and a socketSet, make the descriptor and add it to the set
	// Save the thread pointer, we need to add the set before creating the thread
	if(socketName != NULL && socketSet != NULL){
		addSocketToSet(socketSet, socketName, socketFD, SERVER, thread, connections, socketContext);
	}

    // All done, now loop forever while waiting for connections
    // (Note: it won't consume cpu eternally, accept() is blocking)
	thread = createThread(connectionArgs, listenForConnections, PTHREAD_CREATE_JOINABLE);

	// Now that the thread was created we can set the value in the descriptor
	// (the order has to be like this because the listening for connections needs the socketSet)
	setSocketHandlerThread(socketSet, socketName, thread);

	// Print the socketSet to check things out
//	printSocketSet(socketSet);

	return socketFD;
}


/**
 * Try to resolve each payload from the queue
 * @param queue
 * @param routes
 */
void resolvePayloadQueue(t_queue * queue, t_dictionary * routes, t_socketContext * socketContext){
    t_payload * payload;
    void (*routeHandler)(char **, t_socketContext *);

    if(routes == NULL){
        // If there are no routes just clean the queue (the cast is only to avoid warnings)
        // TODO: Check if this can just be avoided by not having a queue at all
        queue_clean_and_destroy_elements(queue, (void *) destroyPayload);
        return;
    }

    while(!queue_is_empty(queue)){
        // Get the first element from the queue
        payload = queue_pop(queue);

        if(dictionary_has_key(routes, payload->endpoint)){
            logContext(socketContext, info, "Request to endpoint '%s', executing handler", payload->endpoint);
            routeHandler = dictionary_get(routes, payload->endpoint);
            routeHandler(payload->arguments, socketContext);
        }else{
	        logContext(socketContext, warning, "No route found for endpoint '%s'", payload->endpoint);
        }

        // Free the payload memory
        destroyPayload(payload);
    }
}

/**
 * Listen for an incoming message on the socket. This might encompass many payloads
 * @param socketContext
 * @return
 */
int listenForMessage(t_socketContext * socketContext) {
    int socketFD = socketContext->socketFD;
    char buffer[MAXDATASIZE];
    int bytesRead, error = 0;
	bool timedout = false;
    t_queue * queue = queue_create();

    //  A reference to a string
    char ** incompletePayload = malloc(sizeof(char**));
    *incompletePayload = NULL;


    logContext(socketContext, info, "Started listening for messages.");

    while(true){
        // Check the socketContext to see if the thread should terminate
        if(shouldHandlerThreadTerminate(socketContext))
            break;

        // Clean the buffer (not really, but good enough)
        buffer[0] = '\0';

        // Read from the socket buffer
	    if(!timedout){
		    logContext(socketContext, debug, "Reading from socket...");
		    fflush(stdout);
	    }

        bytesRead = recv(socketFD, buffer, MAXDATASIZE-1, 0);
        if (bytesRead == -1) {
	        if(errno == EWOULDBLOCK || errno == EAGAIN){
		        timedout = true;
//		        printf("\b%c", consoleSpinner());
//		        fflush(stdout);
		        continue;
	        }

	        logContext(socketContext, error, "There was an error receiving data, reason: ");
	        error = -1;
	        break;
        }else if(bytesRead == 0){
            logContext(socketContext, warning, "A socket (FD %i) was closed", socketFD);
            error = 0;
            break;
        }

	    timedout = false;

        // End the reading and attempt to unpack the payload
        logContext(socketContext, debug, "Finished reading");
        buffer[bytesRead] = '\0';

	    logContext(socketContext, debug, "Read socket buffer, result: '%s'", buffer);
        parsePayloadsFromBuffer(buffer, queue, &incompletePayload);

        resolvePayloadQueue(queue, socketContext->routes, socketContext);
    }

    // The loop exited for some reason, free vars before returning (the cast is just to avoid warnings)
    queue_destroy_and_destroy_elements(queue, (void *) destroyPayload);
    if(*incompletePayload != NULL)
        free(*incompletePayload);
    free(incompletePayload);
	logContext(socketContext, debug, "Freed all vars from thread");

    return error;
}

/**
 * Wrapper for the listenForMessage function to pass to createThread()
 * @param socketContext
 */
void * messageListener(void * args){
    t_socketContext * socketContext = (t_socketContext*) args;
	logContext(socketContext, info, "Started new listening thread for socket %i", socketContext->socketFD);
    listenForMessage(socketContext); // This will loop until socket closes
    pthread_exit(NULL);
}


/**
 * Creates a server socket with the default connection handler (check that one for doc)
 * @param port
 * @param backlogQueueAmount
 * @return
 */
int createServerSocket(t_socketConfig * config, t_socketSet * socketSet, void * processContext) {
	// Instance the log
	initLog(config->logLevel);
	t_dictionary * routesDict = newRoutesDictionary(config->routes);
    return createServerSocketWithHandler(config->port, BACKLOG, routesDict, messageListener, config->name, socketSet, config->logLevel, processContext);
}


/**
 * Based on beej sendall. Packs a message (string) with the endpoint and arguments, and
 * sends it to the socket provided.
 * @return the result code, -1 if error
 */
int sendPayload(int socketFD, t_payload payload) {
    // Pack the payload
    char * packedPayload = packPayload(payload);
    log_debug(socklog, "Attempting to send packed packed payload '%s'", packedPayload);

    // Initialize vars
    int totalBytesSent = 0;
    int bytesSent = 0;
    int bytesLeft = strlen(packedPayload);

    // Loop while there are bytes remaining to be sent
    while(totalBytesSent < bytesLeft) {
        // Try to send all of them
        bytesSent = send(socketFD, packedPayload + totalBytesSent, (size_t) bytesLeft, 0);

        // Whoops, something went wrong
        if(bytesSent == -1)
            break;

        // All ok, update counters
        totalBytesSent += bytesSent;
        bytesLeft -= bytesSent;
    }

    // Free the packedPayload string after it's sent
    free(packedPayload);

    log_debug(socklog, "Successfully sent %i bytes", totalBytesSent);
    return bytesSent == -1? -1 : 0;
}


/**
 * Send a message through the socket to the specified endpoint
 * @param socketFD
 * @param endpoint
 * @param arguments
 * @return
 */
int sendMessage(int socketFD, char * endpoint, char ** arguments) {
    // Assemble the payload
    t_payload payload;
    payload.arguments = arguments;
    payload.endpoint = endpoint;
    return sendPayload(socketFD, payload);
}


/**
 * Send an array of messages to a socket from the provided socketSet as a payload
 * @param socketSet
 * @param socketName
 * @param endpoint
 * @param arguments
 * @return
 */
int sendMessagesToSocket(t_socketSet *socketSet, char *socketName, char *endpoint, char **arguments) {
	int socketFD = getSocketFD(socketSet, socketName);
	return sendMessage(socketFD, endpoint, arguments);
}


/**
 * Send a message to a socket from the provided socketSet
 * @param socketSet
 * @param socketName
 * @param endpoint
 * @param arguments
 * @return
 */
int sendMessageToSocket(t_socketSet *socketSet, char * socketName, char * endpoint, char * message) {
	int socketFD = getSocketFD(socketSet, socketName);
	char * arguments [2] = { message, NULL };
	return sendMessage(socketFD, endpoint, arguments);
}


/**
 * Send a message to the socket corresponding to the current context
 * @param socketSet
 * @param socketName
 * @param endpoint
 * @param arguments
 * @return
 */
int sendMessagesInContext(t_socketContext * socketContext, char * endpoint, char ** arguments) {
	int socketFD = getSocketFDFromContext(socketContext);
	return sendMessage(socketFD, endpoint, arguments);
}


/**
 * Send a message to the socket corresponding to the current context
 * @param socketSet
 * @param socketName
 * @param endpoint
 * @param arguments
 * @return
 */
int sendMessageInContext(t_socketContext * socketContext, char * endpoint, char * message) {
	char * arguments [2] = { message, NULL };
	return sendMessagesInContext(socketContext, endpoint, arguments);
}


/**
 * Example of a function to pass to create server, that executes
 * when a new client connects to the socket.
 * @param args a void pointer that references an in a list
 * @return
 */
/*void * defaultConnectionHandler(void *args) {
    pthread_t thread;
    void * threadReturnValue;

    // Unpack the handler arguments (the socket FD and the route functions set)
    t_socketContext * socketContext = (t_socketContext*) args;
	logContext(socketContext, info, "Started handler thread for socket %i", socketContext->socketFD);

    // Start listening for incoming messages in a new thread
    thread = createThreadWithContext(socketContext, messageListener, PTHREAD_CREATE_JOINABLE);

    // Wait for listening thread to complete before returning
    pthread_join(thread, &threadReturnValue);

    // Close the socket connection from our side
    close(socketContext->socketFD);

    // If the thread isn't explicitly killed then leaks ahoy
    pthread_exit(NULL);
}*/


/**
 * Create a client socket and connect to a server in a new thread.
 * Also, create a new socketDescriptor and add it to the provided socketSet.
 * @param serverAddress
 * @param port
 * @param routes
 * @param socketName
 * @param socketSet
 * @return the socket file descriptor
 */
int createClientSocket(t_socketConfig * config, t_socketSet * socketSet, void * processContext){
    int socketFD;
    t_addrinfo * serverInfo = NULL;
    pthread_t thread;

	// Initialize the log
	initLog(config->logLevel);

	// Create the client socket and attempt to connect to the server
	socketFD = createSocketAndDoFunc(config->address, config->port, serverInfo, connectClientSocket);

    // We have to do it like this (allocating memory manually) because if we declare it statically then
    // we would pass the pointer to the new thread and once we exit this function since we exit the scope
    // of the variable it would get free'd and we need it elsewhere
    t_socketContext * socketContext = newSocketContext(config->name, config->logLevel);
    socketContext->routes = newRoutesDictionary(config->routes);
    socketContext->socketSet = socketSet;
    socketContext->socketFD = socketFD;
	socketContext->processContext = processContext;

	// Start listening for incoming messages in a new thread
	thread = createThreadWithContext(socketContext, messageListener, PTHREAD_CREATE_JOINABLE);

    // If the user provided a name and a socketSet, make the descriptor and add it to the set
    if(config->name != NULL && socketSet != NULL){
        addSocketToSet(socketSet, config->name, socketFD, CLIENT, thread, NULL, socketContext);
    }

    // Print the socketSet to check things out
//    printSocketSet(socketSet);

    return socketFD;
}


#pragma clang diagnostic pop
