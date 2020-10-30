#include "cpu/exec/helper.h"
#include <nemu.h>
void raise_intr(uint8_t);
make_helper(intr) {
	int NO = instr_fetch(eip + 1, 1);
	cpu.eip += 2;
	print_asm("int %x",NO);
    printf("1\n");
	raise_intr (NO);
	return 0;
}
