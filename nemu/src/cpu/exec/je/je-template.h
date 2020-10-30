#include "cpu/exec/template-start.h"

#define instr je


static void do_execute() {
	DATA_TYPE_S displacement = op_src->val;
	print_asm("je %x",cpu.eip + 1 + DATA_BYTE + displacement);
	if (cpu.ZF == 1)cpu.eip +=displacement;
	print_asm_template1();
}
make_instr_helper(i)


#include "cpu/exec/template-end.h"