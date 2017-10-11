#include <commons/collections/dictionary.h>
#include "../easysocket/commands.h"
#include "../easysocket/libeasysocket.h"
#include "routes.h"
#include "main.h"
#include "functions.h"
#include "../console/functions.h"
#include "kernel.h"


/**
 * The commands that the user will be able to execute
 */
t_commandDescriptor commands[] = {
		{ "ps",    listProcesses,             "Lists all the processes in all queues or one if specified"     },
		{ "pstat", processStats,              "Prints all the stats for a given process (PID)"                },
		{ "gft",   printGlobalFileTable,      "Prints the whole Global File Table"                            },
		{ "chmp",  changeMultiprogrammingDeg, "Changes the degree of multiprogramming to the specified value" },
		{ "pkill", killProcess,               "Kills the specified running process (PID)"                     },
		{ "psch",  pauseScheduling,           "Pauses the process scheduling (no state change until resumed)" },
		{ "rsch",  resumeScheduling,          "Resumes the process scheduling"                                },
		{ NULL }
};


/**
 * The routes that the CONSOLE will be able to send messages to
 */
static t_route consoleRoutes[] = {
		{ INIT_PROCESS_ENDPOINT, initProgram, "Starts a program, initializing everything and running" },
		{ NULL }
};


/**
 * The routes that the FS will be able to send messages to
 */
static t_route fsRoutes[] = {
		{ NULL }
};


/**
 * The routes that the FS will be able to send messages to
 */
static t_route memoryRoutes[] = {
		{ NULL }
};


/**
 * The routes that the CPU will be able to send messages to
 */
static t_route cpuRoutes[] = {
		{ ALLOCATE_MEMORY_ENDPOINT, allocateMemory, "Allocates a memory block and returns the address"    },
		{ FREE_MEMORY_ENDPOINT,     freeMemory,     "Frees a previously alloc'd memory block"             },
		{ OPEN_FILE_ENDPOINT,       openFile,       "Opens a file and returns the FD for that  process"   },
		{ READ_FILE_ENDPOINT,       readFile,       "Reads a file for sp size at offset and returns that" },
		{ WRITE_FILE_ENDPOINT,      writeFile,      "Writes a file with content for sp size at offset"    },
		{ CLOSE_FILE_ENDPOINT,      closeFile,      "Closes an opened file (destroys FD in table)"        },
		{ NULL }
};


/*
 * Main
 */
int main(int argc, char **argv) {
	// Parse the command line arguments
	t_arguments arguments = { DEFAULT_LOG_LEVEL, DEFAULT_CONFIG_LOCATION };
	defaultCommandLineArgumentsParser(argc, argv, PROCESS_NAME, PROCESS_DOCS, &arguments);

	// Initialize the kernel
	t_kernel * kernel = initKernel(config_get_string_value(arguments.config, "GRADO_MULTIPROG"), arguments.loggingLevel);

	// TODO: Initialize shared semaphores from config file (SEM_INIT)
	// TODO: Initialize shared variables from config file

	// Configs for all sockets
	t_socketConfig fsConfig = {
			.address =  config_get_string_value(arguments.config, "IP_FS"),
			.port =     config_get_string_value(arguments.config, "PUERTO_FS"),
			.name =     FS_SOCKET_NAME,
			.routes =   fsRoutes,
			.logLevel = arguments.loggingLevel
	};


	t_socketConfig memoryConfig = {
			.address =  config_get_string_value(arguments.config, "IP_MEMORIA"),
			.port =     config_get_string_value(arguments.config, "PUERTO_MEMORIA"),
			.name =     MEMORY_SOCKET_NAME,
			.routes =   memoryRoutes,
			.logLevel = arguments.loggingLevel
	};

	t_socketConfig consoleConfig = {
			.port =     config_get_string_value(arguments.config, "PUERTO_PROG"),
			.name =     CONSOLE_SOCKET_NAME,
			.routes =   consoleRoutes,
			.logLevel = arguments.loggingLevel
	};

	// Connect to fs and memory
	createClientSocket(&fsConfig, kernel->socketSet, kernel);
	createClientSocket(&memoryConfig, kernel->socketSet, kernel);

	// Create the console client socket and add it to the  socketSet
	createServerSocket(&consoleConfig, kernel->socketSet, kernel);
	// TODO: Create cpu server socket

	// Send a few test messages
	bool forceThreadsToCancel = listenForUserCommands(commands, kernel, arguments.loggingLevel);

	// Wait for all the sockets to end on their own and close/destroy everything
	waitAndCloseAllSocketsAndDestroySet(kernel->socketSet, forceThreadsToCancel);

	// Not really necessary since this would still be reachable when the process ends but whatever
	config_destroy(arguments.config);

	return 0;
}


// TODO: Define planning, 5 queues
// TODO: ^Also, planning algorithms (FIFO, RR w/ Q from config file)
// TODO: Define heap
// TODO: Define GFT
// TODO: Define PFT
// TODO: Remove all logging to console, only to file