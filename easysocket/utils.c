#include <stdio.h>
#include <commons/collections/dictionary.h>
#include <commons/config.h>
#include <commons/string.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <commons/log.h>
#include <ctype.h>

#include "utils.h"


/**
 * Function to pass to a dictionary_iterator()
 * Prints the key and value (assuming the latter is a string)
 * @param key
 * @param value (char *)
 */
void printKeyValue(char * key, void * value){
	printf("\t%s=%s\n", key, (char *) value);
}


/**
 * Prints the provided config struct to console
 * @param config
 */
void printConfig(t_config * config){
	printf("Printing config file:\n");
	dictionary_iterator(config->properties, printKeyValue);
}


/**
 * Gets a char to represent a console spinner
 * Will change with each subsequent call
 * @param pos
 * @return
 */
char consoleSpinner() {
	static int position = -1;
	static char cursor[4] = {'/', '-', '\\', '|'};
	position = (position + 1) % 4;
	return cursor[position];
}


/**
 * Checks is a string is in the provided array (comparing values, not references)
 * @param string
 * @param array
 * @return
 */
bool stringInArray(char * string, int length, char ** array) {
	int  i;
	for(i = 0; i < length; i++) {
		if(!strcmp(string, array[i]))
			return true;
	}
	return false;
}


/**
 * Concatenates the provided strings with the format indicated
 * @param strings
 * @param length
 * @param format
 * @return
 */
char * concatenateWithFormat(char ** strings, int length, char * format) {
	int i;
	char * string = string_duplicate(strings[0]);
	for(i = 1; i < length; i++){
		string_append_with_format(&string, format, strings[i]);
	}
	return string;
}


/**
 * Clears the screen
 * NOTE: This is not portable to non POSIX
 */
void clearScreen(){
	system("clear");
}



/**
 * Reads a file and copies it to a buffer
 * @param path
 * @return the buffer
 */
char * readFileIntoBuffer(char * path){
	// Open the file
	FILE * file = fopen(path, "rb");
	if(file == NULL)
		return NULL;

	// Calculate the length of the file
	fseek(file, 0, SEEK_END);
	long fileSize = ftell(file);
	fseek(file, 0, SEEK_SET);  //same as rewind(f);

	// Allocate the necessary memory and copy
	char * buffer = malloc((size_t) (fileSize + 1));
	fread(buffer, (size_t) fileSize, 1, file);
	fclose(file);
	buffer[fileSize] = '\0';

	return buffer;
}


/*
uint64_t gettid() {
	pthread_t ptid = pthread_self();
	uint64_t threadId = 0;
	memcpy(&threadId, &ptid, std::min(sizeof(threadId), sizeof(ptid)));
	return threadId;
}


void print(char * string) {

	va_list args;
	va_start (args, format);
	vprintf ("[%i] %s", args);
	va_end (args);
	printf("[%i] %s", (unsigned int) pthread_self(), string);
}

void WriteFormatted ( const char * format, ... )
{
	va_list args;
	va_start (args, format);
	vprintf (format, args);
	va_end (args);
}*/
