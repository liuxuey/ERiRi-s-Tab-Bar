#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    cpu.gpr[4]._32-=4;
    swaddr_t addr = instr_fetch(cpu.eip, 1);
    printf("%x\n",addr);
    MEM_W(cpu.esp,cpu.gpr[addr-0x50]._32);
    
}

make_instr_helper(r)


#include "cpu/exec/template-end.h"
