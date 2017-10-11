//
// Created by Alejo on 4/4/2017.
//

#ifndef SIMPLE_EXAMPLE_PAYLOAD_H
#define SIMPLE_EXAMPLE_PAYLOAD_H


#include <commons/collections/queue.h>

#define HEADER_LENGTH 8
#define ENDPOINT_SEPARATOR ";"
#define ARGUMENT_SEPARATOR "\x95"
#define PAYLOAD_END '\0'

typedef struct payload {
    char * endpoint;
    char ** arguments;
} t_payload;

t_payload * newPayload();
t_payload * unpackPayload(char * buffer);
char * packPayload(t_payload payload);
void destroyPayload(t_payload * payload);
void printPayload(t_payload * payload);
void parsePayloadsFromBuffer(char * ogBuffer, t_queue * payloadsQueue, char *** ogIncompletePayload);

#endif //SIMPLE_EXAMPLE_PAYLOAD_H
