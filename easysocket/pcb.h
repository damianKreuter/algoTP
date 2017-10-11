#ifndef SIMPLE_EXAMPLE_PCB_H
#define SIMPLE_EXAMPLE_PCB_H

#define PROCESS_STATES_COUNT 5


/**
 * Exit codes for the program termination (expected or not)
 */
typedef enum {
	EXIT_PROCESS_SUCCESS = 0,
	ERROR_RESOURCE_FAULT = -1,
	ERROR_FILE_NOT_FOUND = -2,
	ERROR_FILE_READ_FORBIDDEN = -3,
	ERROR_FILE_WRITE_FORBIDDEN = -4,
	ERROR_SEG_FAULT = -5,
	EXIT_CONSOLE_DISCONNECT = -6,
	EXIT_CONSOLE_KILL = -7,
	ERROR_ALLOC_EXCEEDS_PAGE = -8,
	ERROR_PAGE_MAX_REACHED = -9,
	ERROR_UNDEFINED = -20
} programExitCode;

/**
 * The states the program can have (also the queue it belongs to)
 */

typedef enum {
	NEW,
	READY,
	EXIT,
	EXEC,
	BLOCK
} programState;

extern const char * const programStateString[];

/**
 * PCB for each program
 */
typedef struct pcb {
	// Following pg.31
	int PID;            // Process identifier
	int programCounter; // Next instruction to be executed
	int codePages;      // Amount of pages used by the process
//	int stackPosition;
	programExitCode exitCode;
	char * script;

	// Custom
	programState state;
} t_pcb;

/**
 * Functions
 */
t_pcb * newPcb();

#endif //SIMPLE_EXAMPLE_PCB_H
