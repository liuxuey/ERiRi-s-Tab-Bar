#include "cpu/exec/helper.h"
#include <nemu.h>

make_helper(intr) {
	int NO = instr_fetch(eip + 1, 1);
	cpu.eip += 2;
	print_asm("int %x",NO);
	return 0;
}