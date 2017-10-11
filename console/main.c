#include <commons/collections/dictionary.h>
#include <stdio.h>
#include <commons/config.h>
#include "functions.h"
#include "routes.h"
#include "main.h"


/**
 * The commands that the user will be able to execute
 */
t_commandDescriptor commands[] = {
	{ "run",   runProgram,         "Runs a program from the provided ANSISOP file"     },
	{ "print", printProgramOutput, "Prints the output of the specified process so far" },
	{ "kill",  killProgram,        "Kills a running program"                           },
	{ NULL }
};


/**
 * The routes that the kernel will be able to send messages to
 */
t_route kernelRoutes[] = {
	{ PRINT_ENDPOINT,        programOutput, "Saves the program output, display with command 'print'" },
	{ INIT_PROGRAM_RESPONSE, initResponse,  "Response from the kernel with the PID"                  },
	{ NULL }
};


/**
 * Main process
 */
int main(int argc, char **argv) {
	// Parse the command line arguments
	t_arguments arguments = { DEFAULT_LOG_LEVEL, DEFAULT_CONFIG_LOCATION };
	defaultCommandLineArgumentsParser(argc, argv, PROCESS_NAME, PROCESS_DOCS, &arguments);

    // Create a new socketSet
    t_socketSet * socketSet = newSocketSet();

	// Get necessary config values from the config, assign them to the context
	t_commandsContext commandsContext;
	commandsContext.kernelIP = config_get_string_value(arguments.config, "IP_KERNEL");
	commandsContext.kernelPort = config_get_string_value(arguments.config, "PUERTO_KERNEL");
	commandsContext.loggingLevel = arguments.loggingLevel;
	commandsContext.kernelRoutes = kernelRoutes;
	commandsContext.config = arguments.config;
	commandsContext.commands = commands;
	commandsContext.socketSet = socketSet;

    // Send a few test messages
	bool forceThreadsToCancel = listenForUserCommands(commands, &commandsContext, arguments.loggingLevel) != 0;

    // Wait for all the sockets to end on their own and close/destroy everything
    waitAndCloseAllSocketsAndDestroySet(socketSet, forceThreadsToCancel);

    // Not really necessary since this would still be reachable when the process ends but whatever
    config_destroy(arguments.config);

    return 0;
}






