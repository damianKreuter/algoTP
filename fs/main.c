#include <commons/collections/dictionary.h>
#include <stdio.h>
#include <commons/config.h>
#include "main.h"
#include "routes.h"


/**
 * The routes that the kernel will be able to send messages to
 */
t_route kernelRoutes[] = {
		{ VALIDATE_FILE_ENDPOINT, validateFile, "Validates that a file exists and returns the boolean" },
		{ CREATE_FILE_ENDPOINT,   createFile,   "Creates a file at the specified path"                 },
		{ DELETE_FILE_ENDPOINT,   deleteFile,   "Deletes a file at the specified path"                 },
		{ READ_FILE_ENDPOINT,     readFile,     "Returns the contents of a file for size at offset"    },
		{ WRITE_FILE_ENDPOINT,    writeFile,    "Writes a buffer of size at offset"                    },
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

	// Get necessary config values from the config
	char * port = config_get_string_value(arguments.config, "PUERTO");

	// Create the server socket the kernel will connect to and add it to the socketSet
	t_socketConfig socketConfig = {
			.port = config_get_string_value(arguments.config, "PUERTO"),
			.name = KERNEL_SOCKET_NAME,
			.logLevel = arguments.loggingLevel,
			.routes = kernelRoutes,
	};
	createServerSocket(&socketConfig, socketSet, NULL);

	// Wait for all the sockets to end on their own and close/destroy everything
	waitAndCloseAllSocketsAndDestroySet(socketSet, false);

	// Not really necessary since this would still be reachable when the process ends but whatever
	config_destroy(arguments.config);

	return 0;
}






