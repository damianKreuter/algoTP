#ifndef SIMPLE_EXAMPLE_CONSOLEROUTES_H
#define SIMPLE_EXAMPLE_CONSOLEROUTES_H

#include "../easysocket/commonstructs.h"


/**
 * Define the endpoints
 */
#define PRINT_ENDPOINT "echo"
#define INIT_PROGRAM_RESPONSE "initresponse"


/**
 * Now the functions
 */
int programOutput(char **arguments, t_socketContext *socketContext);
int initResponse(char **arguments, t_socketContext *socketContext);


#endif //SIMPLE_EXAMPLE_CONSOLEROUTES_H

