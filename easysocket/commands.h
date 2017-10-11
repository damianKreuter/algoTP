//
// Created by Alejo on 5/26/2017.
//

#ifndef SIMPLE_EXAMPLE_COMMANDS_H
#define SIMPLE_EXAMPLE_COMMANDS_H

#include <stdbool.h>
#include <argp.h>
#include <commons/config.h>
#include "commonstructs.h"


extern t_log * commandsLog;
/**
 * Since we'll be using argp in basically the same way in all processes, we can extract that
 */
struct argpDocs {
	char * version;
	char * email;
	char * doc;
	char * argpDocs;
} t_argpDocs;

/**
 * Context that argp will pass to the handler
 */
typedef struct arguments {
	t_log_level loggingLevel;
	char * configPath;
	t_config * config;
} t_arguments;



/**
 * Struct to hold the representation of a command for stdin, and its docs
 */
 typedef struct commandHandler {
	 char * name;
	 // The handler should take (ARGS, CONTEXT)
	 int (*handler)(char **, void *);
	 char * docs;
	 int maxArguments;
 } t_commandDescriptor;

/**
 * Default handler for argp
 * @param key
 * @param arg
 * @param state
 * @return
 */
static error_t argpHandler(int key, char *arg, struct argp_state *state);

error_t defaultCommandLineArgumentsParser(int argc, char ** argv, char * processName, char * docs, t_arguments * arguments);
//void addCommandsToDictionary(t_commandDescriptor * commandHandlers, t_dictionary * dictionary);
int listenForUserCommands(t_commandDescriptor * commands, void * context, t_log_level logLevel);


#endif //SIMPLE_EXAMPLE_COMMANDS_H
