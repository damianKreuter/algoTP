#include <stdio.h>
#include <commons/string.h>
#include "../easysocket/commonstructs.h"
#include "../easysocket/socketcontext.h"

/** This file contains the route handlers that will be called when a message reaches a socket **/



int programOutput(char **arguments, t_socketContext *socketContext) {
	logContext(socketContext, debug, "Echo endpoint, printing arguments");
	string_iterate_lines(arguments, (void (*)(char *)) puts);
	return 0;
}

