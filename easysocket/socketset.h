//
// Created by Alejo on 5/6/2017.
//

#ifndef SIMPLE_EXAMPLE_SOCKETSET_H
#define SIMPLE_EXAMPLE_SOCKETSET_H

#include <commons/collections/dictionary.h>
#include <commons/collections/list.h>
#include <pthread.h>
#include "libeasysocket.h"
#include "commonstructs.h"


t_socketSet * newSocketSet();
int getSocketFD(t_socketSet * socketSet, char * socketName);
int getSocketFDFromContext(t_socketContext * socketContext);
bool setHasSocket(t_socketSet * socketSet, char * socketName);
void printSocketSet(t_socketSet * socketSet);
void lockSocketSet(t_socketSet * socketSet);
void setSocketHandlerThread(t_socketSet * socketSet, char * socketName, pthread_t thread);
void unlockSocketSet(t_socketSet * socketSet);
void waitAndCloseAllSocketsAndDestroySet(t_socketSet * socketSet, bool force);
void addSocketToSet(t_socketSet * socketSet, char * name, int socketFD, socketType type, pthread_t handlerThread, t_list * connections, t_socketContext * socketContext);
void addConnectionToSocket(t_socketSet *socketSet, char *name, int socketFD, pthread_t handlerThread, t_socketContext *socketContext);

#endif //SIMPLE_EXAMPLE_SOCKETSET_H
