#ifndef SIMPLE_EXAMPLE_MAIN_MEMORY_H
#define SIMPLE_EXAMPLE_MAIN_MEMORY_H

#include "../easysocket/libeasysocket.h"
#include "../easysocket/commands.h"


// Defaults
#define PROCESS_NAME "MEMORY"
#define PROCESS_DOCS "[FILENAME]..."
#define DEFAULT_CONFIG_LOCATION "./configs/memory.txt"
#define DEFAULT_LOG_LEVEL LOG_LEVEL_DEBUG

// Custom
#define KERNEL_SOCKET_NAME "kernel"

// This is separately defined for each process so as to not clutter all of them
typedef struct commandsContext {
	char * kernelIP;
	char * kernelPort;
	t_config * config;
	t_log_level loggingLevel;
	t_socketSet * socketSet;
	t_commandDescriptor * commands;
	t_route * kernelRoutes;
} t_commandsContext;

#endif //SIMPLE_EXAMPLE_MAIN_MEMORY_H
