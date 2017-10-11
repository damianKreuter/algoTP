
#ifndef FUNCIONES_COMUNES_H_
#define FUNCIONES_COMUNES_H_

#include <commons/config.h>

/**
 * Macro function to add context to a function that will be passed to a higher order one
 */
#define bindContext(function, boundFunction, arg, rest...) void boundFunction(void * arg) { function(arg, rest); }

/**
 * Returns the size of a static array
 */
#define sizeofArray(array) sizeof(array) / sizeof(array[0])
#define COUNT_OF(x) ((sizeof(x)/sizeof(0[x])) / ((size_t)(!(sizeof(x) % sizeof(0[x])))))



void printConfig(t_config * config);
char consoleSpinner();
bool stringInArray(char * string, int length, char ** array);
char * concatenateWithFormat(char ** strings, int length, char * format);
void clearScreen();
char * readFileIntoBuffer(char * path);
//void print(char * string);

#endif /* FUNCIONES_COMUNES_H_ */
