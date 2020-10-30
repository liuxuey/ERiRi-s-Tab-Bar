#include "cpu/exec/template-start.h"

#define instr scas

static void do_execute () {
	swaddr_t s1 = REG(R_EAX), s2 = swaddr_read(reg_l(R_EDI), DATA_BYTE);
	uint32_t res = s1 - s2;
	printf("%d\n", res);
	if (cpu.DF == 0) reg_l(R_EDI) += DATA_BYTE;
	else reg_l(R_EDI) -= DATA_BYTE;
	concat(updateCPU_, SUFFIX) (res);
	int len = (DATA_BYTE << 3) - 1;
	cpu.CF = s1 < s2;
    cpu.OF = ((s1 >> len) != (s2 >> len) && (s2 >> len) == cpu.SF);
	print_asm("scas");
}
make_instr_helper(r)
#include "cpu/exec/template-end.h"