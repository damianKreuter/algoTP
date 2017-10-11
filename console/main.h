#ifndef SIMPLE_EXAMPLE_MAIN_CONSOLE_H
#define SIMPLE_EXAMPLE_MAIN_CONSOLE_H

#include <commons/config.h>
#include <commons/log.h>
#include "../easysocket/commonstructs.h"
#include "../easysocket/commands.h"
#include "../easysocket/libeasysocket.h"


// Defaults
#define PROCESS_NAME "Console"
#define PROCESS_DOCS "[FILENAME]..."
#define DEFAULT_CONFIG_LOCATION "./configs/console.txt"
#define DEFAULT_LOG_LEVEL LOG_LEVEL_DEBUG

// Custom
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


#endif //SIMPLE_EXAMPLE_MAIN_CONSOLE_H
