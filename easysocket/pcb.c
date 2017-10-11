#include <stdlib.h>
#include "pcb.h"

const char * const programStateString[] = {
	"NEW", "READY", "EXIT", "EXEC", "BLOCK"
};


/**
 * Make a new pcb
 * @return
 */
t_pcb * newPcb() {
	t_pcb * pcb = malloc(sizeof(t_pcb));
	pcb->state = NEW;
	return pcb;
}