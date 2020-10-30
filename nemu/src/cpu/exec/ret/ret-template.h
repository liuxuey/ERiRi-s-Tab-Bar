#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_n_, SUFFIX))
{
	cpu.eip = MEM_R (cpu.esp); //读取esp里面存放的eip的值
	//if (DATA_BYTE == 2)cpu.eip &= 0xffff;
	cpu.esp += DATA_BYTE;
	print_asm("ret");
	// printf("%d\n",cpu.eax);
	return 1;
}

make_helper(concat(ret_i_, SUFFIX))
{
	int val = instr_fetch(eip + 1, 2);
	int i;
	cpu.eip = MEM_R (cpu.esp);
	if (DATA_BYTE == 2)cpu.eip &= 0xffff;
	cpu.esp += DATA_BYTE;
	for (i = 0;i < val; i+=DATA_BYTE)
	MEM_W (cpu.esp + i,0);
	cpu.esp += val;
	print_asm("ret $0x%x",val);
	return 1;
}

#include "cpu/exec/template-end.h"