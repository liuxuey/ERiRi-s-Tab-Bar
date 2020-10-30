#include "cpu/exec/template-start.h"

#define instr int


static void do_execute() {
	cpu.eip+=2;
}



#include "cpu/exec/template-end.h"