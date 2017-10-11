#include <commons/collections/dictionary.h>
#include <stdio.h>
#include <commons/config.h>
#include "main.h"
#include "routes.h"


/**
 * The commands that the user will be able to execute
 */
t_commandDescriptor commands[] = {
		{ NULL }
};


/**
 * The routes that the kernel will be able to send messages to
 */
t_route kernelRoutes[] = {
		{ "echo", programOutput, "Prints the incoming payload" },
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
	commandsContext.kernelIP = config_get_string_value(arguments.config, "IP_CONSOLA");
	commandsContext.kernelPort = config_get_string_value(arguments.config, "PUERTO_CONSOLA");
	commandsContext.loggingLevel = arguments.loggingLevel;
	commandsContext.kernelRoutes = kernelRoutes;
	commandsContext.config = arguments.config;
	commandsContext.commands = commands;
	commandsContext.socketSet = socketSet;

	// Create the client socket and add it to the  socketSet
	//	createClientSocket(ip, port, kernelRoutes, sizeofArray(kernelRoutes), KERNEL_SOCKET_NAME, socketSet, arguments.loggingLevel);

	// Send a few test messages
	bool forceThreadsToCancel = listenForUserCommands(commands, &commandsContext, arguments.loggingLevel) != 0;

	// Wait for all the sockets to end on their own and close/destroy everything
	waitAndCloseAllSocketsAndDestroySet(socketSet, forceThreadsToCancel);

	// Not really necessary since this would still be reachable when the process ends but whatever
	config_destroy(arguments.config);

	return 0;
}






