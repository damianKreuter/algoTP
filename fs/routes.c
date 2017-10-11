/**
 * This file contains the route handlers that will be called when a message reaches a socket
 */

#include <libeasysocket.h>

/*
 * KERNEL ROUTES
 */


/**
 * Validates that a file exists and returns the boolean
 * @param arguments [path]
 * @from KERNEL
 */
int validateFile(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
    return 0;
}


/**
 * Creates a file at the specified path
 * @param arguments [path]
 * @from
 */
int createFile(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
    return 0;
}


/**
 * Deletes a file at the specified path
 * @params arguments [path]
 * @from KERNEL
 */
int deleteFile(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
    return 0;
}


/**
 * Returns the contents of a file for size at offset
 * @params arguments [path, offset, size]
 * @from KERNEL
 */
int readFile(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
    return 0;
}


/**
 * Writes a buffer of size at offset
 * @params arguments [path, offset, size, buffer]
 * @from KERNEL
 */
int writeFile(char ** arguments, t_socketContext * socketContext){
    // TODO: Complete
    return 0;
}