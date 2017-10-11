


#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <argp.h>
#include <stdlib.h>
#include <commons/config.h>
#include <commons/string.h>
#include "commonstructs.h"
#include "commands.h"
#include "utils.h"

#define MAX_MESSAGE_LENGTH 100
#define MAX_COMMAND_LENGTH 254
#define LOG_OUTPUT_FILE "out.txt"
#define PROGRAM_NAME "commandlib"
#define COMMAND_SEPARATOR " "
#define COMMAND_DOCS_WIDTH 15 // This should be calculated but I'm lazy

static char * EXIT_COMMANDS[] =  { "exit",  "quit", "q" };
static char * HELP_COMMANDS[] =  { "help",  "h" };
static char * CLEAR_COMMANDS[] = { "clear", "c" };

static struct argp_option defaultArgpOptions[] = {
		{ "logging", 'l', "LEVEL",       0, "Set the logging level for the process."},
		{ "config",  'c', "CONFIG_PATH", 0, "Specify the configuration file location."},
		{ 0 }
};

t_log * commandsLog = NULL;

/**
 *	Default handler for argp
 *
 * @param key
 * @param arg
 * @param state
 * @return
 */
static error_t defaultArgpHandler(int key, char *arg, struct argp_state *state) {
	t_arguments * arguments = state->input;
	switch (key) {
		case 'l':
			printf("Setting log level to '%s'\n", arg);
			arguments->loggingLevel = log_level_from_string(arg);
			if(arguments->loggingLevel == -1){
				printf("The provided logging level is not valid.\n");
				exit(1);
			}
			break;
		case 'c':
			printf("Setting config path to '%s'\n", arg);
			arguments->configPath = arg;
			break;
		case ARGP_KEY_ARG:
			return 0;
		default:
			return ARGP_ERR_UNKNOWN;
	}
	return 0;
}


/**
 * Use this to parse the command line arguments in the main processes.
 *
 * @param argc
 * @param argv
 * @param processName
 * @param docs
 * @param arguments
 * @return
 */
error_t defaultCommandLineArgumentsParser(int argc, char ** argv, char * processName, char * docs, t_arguments * arguments){
	struct argp argp = { defaultArgpOptions, defaultArgpHandler, docs, processName };

	printf("\nParsing arguments...\n");
	error_t err = argp_parse(&argp, argc, argv, 0, 0, arguments);
	printf("All done parsing arguments.\n\n");

	arguments->config = config_create(arguments->configPath);
	printConfig(arguments->config);
	printf("\n");
	return err;
}


/**
 * Iterates over a static array of commandDescriptors
 * @param commands
 * @param function
 */
void iterateCommandArray(t_commandDescriptor * commands, void (*function)(t_commandDescriptor, void * context), void * context){
	int i;
	for(i = 0; commands[i].name != NULL; i++){
		t_commandDescriptor command = commands[i];
		function(command, context);
	}
}



//void addCommandToDictionary(t_commandDescriptor command, void * context) {
//	t_dictionary * dictionary = (t_dictionary *) context;
//	log_trace(log, "Adding command '%s'", command.name);
//	dictionary_put(dictionary, command.name, command.handler);
//}
//
///**
// * Create a dictionary from an array of commandDescriptors
// * @param commands
// * @return
// */
//t_dictionary * createCommandDictionary(t_commandDescriptor * commands, int commandsLength){
//	t_dictionary * dictionary = dictionary_create();
//	log_trace(log, "Adding commands to dictionary");
//	iterateCommandArray(commands, commandsLength, addCommandToDictionary, dictionary);
//	log_trace(log, "Done adding commands to dictionary");
//	return dictionary;
//}


/**
 * Prints a command descriptor to console (not via log.h)
 * @param command
 * @param context
 */
void printCommandDescriptor(t_commandDescriptor command, void * context) {
	printf("\t%-*s   %s\n", COMMAND_DOCS_WIDTH, command.name, command.docs);
}


/**
 * Prints the whole command docs to console (not via log.h)
 * @param commands
 * @param commandsLength
 */
void printCommandDocs(t_commandDescriptor * commands) {
	char * concatenatedHelpCommands  = concatenateWithFormat(HELP_COMMANDS,  sizeofArray(HELP_COMMANDS),  ", %s");
	char * concatenatedExitCommands  = concatenateWithFormat(EXIT_COMMANDS,  sizeofArray(EXIT_COMMANDS),  ", %s");
	char * concatenatedClearCommands = concatenateWithFormat(CLEAR_COMMANDS, sizeofArray(CLEAR_COMMANDS), ", %s");
	printf("\nAvailable commands:\n"
	       "\t%-*s   Prints this information\n"
	       "\t%-*s   Exits the command listener\n"
	       "\t%-*s   Clears the console screen\n",
	       COMMAND_DOCS_WIDTH, concatenatedHelpCommands,
	       COMMAND_DOCS_WIDTH, concatenatedExitCommands,
		   COMMAND_DOCS_WIDTH, concatenatedClearCommands);
	iterateCommandArray(commands, printCommandDescriptor, NULL);
	printf("\n");
	free(concatenatedHelpCommands);
	free(concatenatedClearCommands);
	free(concatenatedExitCommands);
}


/**
 * Gets a command with the desired name from the provided descriptor array
 * @param commandName
 * @param commands
 * @param commandsLength
 * @return
 */
t_commandDescriptor * getCommandDescriptor(char * commandName, t_commandDescriptor * commands) {
	int i;
	for(i = 0; commands[i].name != NULL; i++){
		if(!strcmp(commands[i].name, commandName)){
			return &(commands[i]);
		}
	}
	return NULL;
}


/**
 * Start listening for input commands, and handle them appropriately
 * @param commands
 * @param context
 * @param logLevel
 * @return
 */
int listenForUserCommands(t_commandDescriptor * commands, void * context, t_log_level logLevel) {
	commandsLog = log_create(LOG_OUTPUT_FILE, PROGRAM_NAME, true, logLevel);
	char commandLine[MAX_COMMAND_LENGTH];
	int result = 0;

	while(result == 0){
		printf("> Enter a valid command (or 'help' to see the list of available ones): \n");
		fgets(commandLine, MAX_COMMAND_LENGTH, stdin);

		// Remove the line break
		char * lastChar = &(commandLine[strlen(commandLine) -1]);
		if (*lastChar == '\n')
			*lastChar = '\0';

		// Check if the command is empty
		log_trace(commandsLog, commandLine);
		if(commandLine[0] == '\0')
			continue;

		// If we get the exit command stop listening for them and quit
		if(stringInArray(commandLine, sizeofArray(EXIT_COMMANDS), EXIT_COMMANDS)){
			log_info(commandsLog, "Exiting command listener...");
			result = -1;
			break;
		}

		// If we get the help command list the available commands
		if(stringInArray(commandLine, sizeofArray(HELP_COMMANDS), HELP_COMMANDS)){
			printCommandDocs(commands);
			continue;
		}

		// If we get the clear command clear the screen
		if(stringInArray(commandLine, sizeofArray(CLEAR_COMMANDS), CLEAR_COMMANDS)){
			clearScreen();
			continue;
		}

		// Split the line by the separator and also do so for the command and arguments
		char ** splitLine = string_split(commandLine, COMMAND_SEPARATOR);
		char * command = splitLine[0];
		char ** arguments = splitLine + 1;

		// Get the command descriptor
		t_commandDescriptor * commandDescriptor = getCommandDescriptor(command, commands);

		if(commandDescriptor != NULL){
			log_debug(commandsLog, "Command found for '%s'", command);
//			int (*handler)(char **, void*) = (int (*)(char **, void*)) dictionary_get(commandSet, command);
			result = commandDescriptor->handler(arguments, context);
		} else {
			log_warning(commandsLog, "The entered command does not exist.");
		}

		// Free the strings from the split
		string_iterate_lines(splitLine, (void(*)(char*)) free);
		free(splitLine);
	}

	// Free what needs to be freed
//	dictionary_destroy(commandSet);
	log_destroy(commandsLog);

	return result;
}