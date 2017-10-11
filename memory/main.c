#include <commons/collections/dictionary.h>
#include <stdio.h>
#include <commons/config.h>
#include "main.h"
#include "routes.h"
#include "functions.h"


/**
 * The commands that the user will be able to execute
 */
t_commandDescriptor commands[] = {
		{ "delay", setDelay,   "Sets the fake delay for main memory access"         },
		{ "dump",  dumpStats,  "Prints all the stats to console"                    },
		{ "flush", flushCache, "Wipes all data from the cache"                      },
		{ "size",  printSizes, "Prints size of memory or a specified process (PID)" },
		{ NULL }
};


/**
 * The routes that the kernel will be able to send messages to
 */
t_route kernelRoutes[] = {
		{ INIT_PROCESS_ENDPOINT, initProgramMemory, "Initial memory assignment for a new process" },
		{ ASSIGN_PAGE_ENDPOINT,  assignPages,       "Assigns more pages to a running process"     },
		{ END_PROCESS_ENDPOINT,  endProgramMemory,  "Frees all memory assigned to a process"      },
		{ NULL }
};


/**
 * The routes that the cpu will be able to send messages to
 */
t_route cpuRoutes[] = {
		{ READ_MEMORY_ENDPOINT,  readMemory,  "Reads the required bytes from memory and returns"    },
		{ WRITE_MEMORY_ENDPOINT, writeMemory, "Writes the provided bytes to memory at page->offset" },
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

	// Config for kernel socket
	t_socketConfig kernelConfig = {
			.port = config_get_string_value(arguments.config, "PUERTO"),
			.name = KERNEL_SOCKET_NAME,
			.routes = kernelRoutes,
			.logLevel = arguments.loggingLevel
	};

	// Create the client socket and add it to the  socketSet
	createServerSocket(&kernelConfig, socketSet, NULL);

	// Send a few test messages
	bool forceThreadsToCancel = listenForUserCommands(commands, &commandsContext, arguments.loggingLevel) != 0;

	// Wait for all the sockets to end on their own and close/destroy everything
	waitAndCloseAllSocketsAndDestroySet(socketSet, forceThreadsToCancel);

	// Not really necessary since this would still be reachable when the process ends but whatever
	config_destroy(arguments.config);

	return 0;
}






