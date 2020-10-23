#include "cpu/exec/template-start.h"

#define instr call

static void do_execute() {
    cpu.gpr[4]._32-=4;
    printf("here1");
    swaddr_t addr = instr_fetch(cpu.eip + 1, 1);
    cpu.eip+=5;
    cpu.eip+=addr;
}

make_instr_helper(i)


#include "cpu/exec/template-end.h"
