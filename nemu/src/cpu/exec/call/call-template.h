#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    cpu.gpr[4]._32-=4;
    
    swaddr_t addr = instr_fetch(cpu.eip + 1, 1);
    printf("here1\n");
    cpu.eip+=5;
    printf("here1\n");
    cpu.eip+=addr;
    printf("%x\n",cpu.eip);
}

make_instr_helper(i)


#include "cpu/exec/template-end.h"
