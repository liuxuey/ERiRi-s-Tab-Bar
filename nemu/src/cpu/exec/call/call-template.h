#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    cpu.gpr[4]._32-=4;
    MEM_W(cpu.esp,cpu.eip);
    swaddr_t addr = instr_fetch(cpu.eip + 1, 1);
    cpu.eip+=addr;
    printf("%x\n",cpu.eip);
}

make_instr_helper(i)


#include "cpu/exec/template-end.h"
