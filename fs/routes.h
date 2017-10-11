#ifndef SIMPLE_EXAMPLE_FSROUTES_H
#define SIMPLE_EXAMPLE_FSROUTES_H

#include <commonstructs.h>


/**
 * Define the endpoints
 */
#define VALIDATE_FILE_ENDPOINT "validate"
#define CREATE_FILE_ENDPOINT "create"
#define DELETE_FILE_ENDPOINT "delete"
#define READ_FILE_ENDPOINT "read"
#define WRITE_FILE_ENDPOINT "write"


/**
 * Now the functions
 */
int validateFile(char ** arguments, t_socketContext * socketContext);
int createFile(char ** arguments, t_socketContext * socketContext);
int deleteFile(char ** arguments, t_socketContext * socketContext);
int readFile(char ** arguments, t_socketContext * socketContext);
int writeFile(char ** arguments, t_socketContext * socketContext);


#endif //SIMPLE_EXAMPLE_FSROUTES_H

