#include "cpu/exec/template-start.h"

#define instr leave

static void do_execute () {
	swaddr_t i;
	current_sreg = R_SS;
	for (i = cpu.esp;i < cpu.ebp; i+=DATA_BYTE)
		MEM_W (i,0);
	cpu.esp = cpu.ebp;
	cpu.ebp = MEM_R (cpu.esp);
	cpu.esp+=DATA_BYTE;
	print_asm("leave");
}
make_instr_helper(n)


#include "cpu/exec/template-end.h"