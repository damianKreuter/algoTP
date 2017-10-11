#include <commons/string.h>
#include <commons/collections/queue.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <commons/log.h>

#include "payload.h"
#include "libeasysocket.h"


/**
 * Creates a new payload and returns it
 * (It doesn't need to alloc anything, it has a fixed size, though not it's properties)
 * @return the new payload
 */
t_payload * newPayload(){
    t_payload *payload = malloc(sizeof(t_payload));
    payload->arguments = NULL;
    payload->endpoint = NULL;
    return payload;
}


/**
 * Frees an array of strings
 */
void freeStringArray(char ** array) {
    int i = 0;

    while(array[i] != NULL){
        free(array[i++]);
    }
    free(array);
}


/**
 * Takes a pointer to a payload and frees all it's stuff
 * @param payload
 */
void destroyPayload(t_payload * payload){
    if(payload->endpoint != NULL) free(payload->endpoint);
    if(payload->arguments != NULL) freeStringArray(payload->arguments);
    free(payload);
}


/**
 * Convrt an integer to a 1 byte, 4 char array
 * @param n
 * @return
 */
unsigned char * intToByteArray(unsigned int n) {
    char * arr = malloc(sizeof(char) * (HEADER_LENGTH + 1));
    char * itoa;
    int i, aux;
//    unsigned char * bytes = malloc(sizeof(char) * 4);
//
//    bytes[0] = n & 0xFF;
//    bytes[1] = (n >> 8) & 0xFF;
//    bytes[2] = (n >> 16) & 0xFF;
//    bytes[3] = (n >> 24) & 0xFF;

//    return bytes;

//    log_trace(tLog, "The int is %i\n", n);
    itoa = string_itoa(n);
//    log_trace(tLog, "Itoa is '%s'\n", itoa);
    aux = strlen(itoa) - HEADER_LENGTH;
//    log_trace(tLog, "Aux is %i\n", n);
    for (i = 0; i < HEADER_LENGTH; ++i) {
        if(aux > -1)
            arr[i] = itoa[aux];
        else
            arr[i] = '0';
        aux++;
    }
    arr[i] = '\0';
//    log_trace(tLog, "The int to byte is '%s'\n", arr);
    free(itoa);
    return arr;
}


unsigned int byteArrayToIntFrom(unsigned char * bytes, unsigned int from) {
    unsigned int n;
    char* substring;
//
//    log_trace(tLog, "BYTE ARRAY TO INT FROM\n");
//    log_trace(tLog, "%x\n", bytes[from]);
//    n = bytes[from++];
//    n |= bytes[from++] >> 8;
//    n |= bytes[from++] >> 16;
//    n |= bytes[from] >> 24;
//
//    return n;
    substring = string_substring(bytes, from, HEADER_LENGTH);
    n = atoi(substring);
    free(substring);
    return n;
}


/**
 * Convert a 4 char/1 byte array to an unsigned int
 * @param bytes
 * @return
 */
unsigned int byteArrayToInt(unsigned char * bytes) {
    return byteArrayToIntFrom(bytes, 0);
}


/**
 * Helper function for printPayload()
 * @param arg
 */
void printArgument(char * arg) {
    log_trace(socklog, ">> Arg: '%s'", arg);
}


/**
 * Prints a payload (it's properties) to console
 * @param payload
 */
void printPayload(t_payload * payload){
    log_trace(socklog, "Printing payload:");
	log_trace(socklog, "> Endpoint: '%s'", payload->endpoint);
    if(payload->arguments != NULL)
        string_iterate_lines(payload->arguments, printArgument);
}


/**
 * Packs the provided payload
 */
char * packPayload(t_payload payload) {
    int length;
    char * lengthByteArray;

    // Initialize strings, start payload with the endpoint
    char * packedPayload = string_duplicate(payload.endpoint);

	// Separate the endpoint from the arguments with ENDPOINT_SEPARATOR
	string_append(&packedPayload, ENDPOINT_SEPARATOR);

	// If there are no arguments no need to do this
	if(payload.arguments != NULL && payload.arguments[0] != NULL){
		log_trace(socklog, "There are arguments to pack");
		char * packedArgs = string_new();

		/**
		 * This function must be defined nested here to be able to have context within it
		 * (the packedArgs variable exists within the scope)
		 */
		void concatArgs(char * arg) {
			string_append_with_format(&packedArgs, "%s%s", arg, ARGUMENT_SEPARATOR);
		}

		// Concat the arguments, separate them by ARGUMENT_SEPARATOR
		string_iterate_lines(payload.arguments, concatArgs);

		// Replace the last comma with PAYLOAD_END
		packedArgs[strlen(packedArgs) - 1] = PAYLOAD_END;
		// Append the endpoint to the arguments
		string_append(&packedPayload, packedArgs);
		free(packedArgs);
	}

	// Get the length of the payload
	length = strlen(packedPayload);
	// Convert the length to a byte array (actually there is no byte array in C, it's just a char*)
	lengthByteArray = intToByteArray(length);
	// Append with custom method, strcat ends with nulls
	string_append(&lengthByteArray, packedPayload);
	// Free what isn't needed anymore
    free(packedPayload);
    return lengthByteArray;
}


/**
 * Receives a buffer string and parses whatever payloads it contains.
 * If it can't fully read a payload it'll save it to the incomplete payload pointer.
 * If there is something in that same pointer when the function is called it will append it
 * to the provided buffer and parse the whole thing, that way it can solve payloads fragmented
 * into several buffer reads.
 * @param buffer
 * @param payloadsQueue
 * @param incompletePayload
 */
void parsePayloadsFromBuffer(char * ogBuffer, t_queue * payloadsQueue, char *** ogIncompletePayload) {
    int payloadLength,
        bufferLength,
        remainingBufferLength,
        currentBufferPosition = 0;
    char * payloadString = NULL;
    char * buffer;
    t_payload * payload;

    if(*ogIncompletePayload == NULL || **ogIncompletePayload == NULL){
        // Else just copy the original buffer
        log_trace(socklog, "No incomplete payload, just duplicating buffer");
        buffer = string_duplicate(ogBuffer);
    } else {
        // Append the incomplete payload to the buffer if there is one
        log_trace(socklog, "There was an incomplete payload '%s', appending to buffer copy", **ogIncompletePayload);
        buffer = string_duplicate(**ogIncompletePayload);
        free(**ogIncompletePayload);
        **ogIncompletePayload = NULL;
        string_append(&buffer, ogBuffer);
    }

    // Set the buffer length
    bufferLength = strlen(buffer);

    // Loop while the buffer has data
    while(currentBufferPosition < bufferLength) {
        // Get the length of the first payload
        // (Even though we're passing the whole thing the func will only read the first byte)
        payloadLength = byteArrayToIntFrom(buffer, currentBufferPosition);
        log_trace(socklog, "The payload length is %i", payloadLength);

        // Calculate the remaining buffer length
        remainingBufferLength = bufferLength - currentBufferPosition - HEADER_LENGTH;

        if (remainingBufferLength < payloadLength) {
            // If we can't read enough bytes from the buffer to complete the payload, then
            // leave it as incomplete and process it next time
            log_trace(socklog, "Remaining buffer length (%i) < payload (%i), leaving it incomplete", remainingBufferLength, payloadLength);
            log_trace(socklog, "Incomplete payload before reassigning: '%s'", **ogIncompletePayload);
            **ogIncompletePayload = string_substring_from(buffer, currentBufferPosition);
            log_trace(socklog, "Incomplete payload copied: '%s'", **ogIncompletePayload);
            break;
        }

        // Offset the current position by the size of the length header we just read
        currentBufferPosition += HEADER_LENGTH;

        // There are enough bytes in the buffer for the payload, get them and unpack
        payloadString = string_substring(buffer, currentBufferPosition, payloadLength);
        log_trace(socklog, "The payloadString is '%s'", payloadString);
        payload = unpackPayload(payloadString);

        // Add them to the queue to process later
        log_trace(socklog, "Adding new payload to the queue:");
        printPayload(payload);

        queue_push(payloadsQueue, payload);
//        destroyPayload(payload);
        // Update the current buffer position
        currentBufferPosition += payloadLength;

        free(payloadString);
    }

    free(buffer);
}


/**
 * Tries to make a payload from the provided string
 */
t_payload * unpackPayload(char * buffer) {
    // Declare an aux array and make a new payload
    char ** auxArray;
//    char * payloadLengthBytes;
    t_payload *payload = newPayload();

    log_trace(socklog, "Received packed payload '%s'; Attempting to unpack...", buffer);
    // Get the length
//    payloadLengthBytes = byteArrayToInt(buffer);
//    log_trace(tLog, "The first part of the string is '%s'", payloadLengthBytes);
//    log_trace(tLog, "The payload length is %i\n", byteArrayToInt(buffer));

    // Get the endpoint
    auxArray = string_n_split(buffer, 2, ENDPOINT_SEPARATOR);
    payload->endpoint = auxArray[0];

    // Split the rest by the ARGUMENT_SEPARATOR
    payload->arguments = auxArray[1] == NULL? NULL : string_split(auxArray[1], ARGUMENT_SEPARATOR);

    // Free the aux array (not auxArray[0] because we're using that in the payload)
    free(auxArray[1]);
    free(auxArray);
    return payload;
}