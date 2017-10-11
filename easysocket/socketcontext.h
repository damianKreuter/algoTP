//
// Created by Alejo on 5/21/2017.
//

#ifndef SIMPLE_EXAMPLE_SOCKETCONTEXT_H
#define SIMPLE_EXAMPLE_SOCKETCONTEXT_H

#include "commonstructs.h"

// Macro to make the logging easier when dealing with the log inside socketContext
// WARNING: the ## before the variadic arguments is gcc specific
#define logContext(socketContext, level, string, rest...) log_ ## level(socketContext->log, string, ## rest)

t_socketContext * newSocketContext(char * socketName, t_log_level logLevel);
t_socketContext * copySocketContext(t_socketContext * socketContext, int newSocketFD);
bool shouldHandlerThreadTerminate(t_socketContext *socketContext);

#endif //SIMPLE_EXAMPLE_SOCKETCONTEXT_H
