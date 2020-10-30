#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute () {
	if (DATA_BYTE == 1)
	{
		current_sreg = R_SS;
		swaddr_write (op_src->addr, 4 ,MEM_R(cpu.esp));
		MEM_W(cpu.esp , 0);
		cpu.esp += 4;
	}
	else
	{
		current_sreg = R_SS;
		OPERAND_W (op_src, MEM_R(cpu.esp));
		MEM_W(cpu.esp , 0);
		cpu.esp += DATA_BYTE;
	}
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
make_instr_helper(rm)
#endif

#include "cpu/exec/template-end.h"