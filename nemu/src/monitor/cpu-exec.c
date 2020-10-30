#include "monitor/monitor.h"
#include "cpu/helper.h"
#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "common.h"
#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <setjmp.h>

/* The assembly code of instructions executed is only output to the screen
 * when the number of instructions executed is less than this value.
 * This is useful when you use the `si' command.
 * You can modify this value as you want.
 */
#define MAX_INSTR_TO_PRINT 10
WP *retwppool();
WP *rethead();
int nemu_state = STOP;
void ui_mainloop();
int exec(swaddr_t);

char assembly[80];
char asm_buf[128];

/* Used with exception handling. */
jmp_buf jbuf;

static inline void push (int val)
{
	current_sreg = R_SS;
	reg_l (R_ESP) -= 4;
	swaddr_write (reg_l (R_ESP) , 4 , val);
}
void raise_intr(uint8_t NO) {
	/* TODO: Trigger an interrupt/exception with ``NO''.
	 * That is, use ``NO'' to index the IDT.
	 */
    	GATE_descriptor gate;
	idt_des = &gate;
   	lnaddr_t pidt = cpu.idtr.base_addr + NO * 8;
   	idt_des->first_part = lnaddr_read(pidt, 4);
	idt_des->second_part = lnaddr_read(pidt + 4, 4);
	Assert ((NO << 3) <= cpu.idtr.seg_limit,"idt out limit %hd, %d", (NO<<3), cpu.idtr.seg_limit);
	push (cpu.eflags);
	if (cpu.cr0.protect_enable == 0)
	{
		cpu.IF = 0;
		cpu.TF = 0;
	}
	push (cpu.cs.selector);
	push (cpu.eip); 
    	cpu.cs.selector = idt_des -> segment;
    	Assert(((cpu.cs.selector>>3)<<3) <= cpu.gdtr.seg_limit, "segment out limit %d, %d", ((cpu.cs.selector>>3)<<3), cpu.gdtr.seg_limit);
	current_sreg = R_CS;
    	sreg_load();
    	cpu.eip = cpu.cs.seg_base + idt_des -> offset_15_0 + (idt_des -> offset_31_16 << 16);
    /* Jump back to cpu_exec() */
    	longjmp(jbuf, 1);
}

void print_bin_instr(swaddr_t eip, int len) {
	int i;
	int l = sprintf(asm_buf, "%8x:   ", eip);
	for(i = 0; i < len; i ++) {
		l += sprintf(asm_buf + l, "%02x ", instr_fetch(eip + i, 1));
	}
	sprintf(asm_buf + l, "%*.s", 50 - (12 + 3 * len), "");
}

/* This function will be called when an `int3' instruction is being executed. */
void do_int3() {
	printf("\nHit breakpoint at eip = 0x%08x\n", cpu.eip);
	nemu_state = STOP;
}

/* Simulate how the CPU works. */
void cpu_exec(volatile uint32_t n) {
	if(nemu_state == END) {
		printf("Program execution has ended. To restart the program, exit NEMU and run again.\n");
		return;
	}
	nemu_state = RUNNING;

#ifdef DEBUG
	volatile uint32_t n_temp = n;
#endif

	setjmp(jbuf);

	for(; n > 0; n --) {
#ifdef DEBUG
		swaddr_t eip_temp = cpu.eip;
		if((n & 0xffff) == 0) {
			/* Output some dots while executing the program. */
			fputc('.', stderr);
		}
#endif

		/* Execute one instruction, including instruction fetch,
		 * instruction decode, and the actual execution. */
		int instr_len = exec(cpu.eip);

		cpu.eip += instr_len;

#ifdef DEBUG
		print_bin_instr(eip_temp, instr_len);
		strcat(asm_buf, assembly);
		Log_write("%s\n", asm_buf);
		if(n_temp < MAX_INSTR_TO_PRINT) {
			printf("%s\n", asm_buf);
		}
#endif

		/* TODO: check watchpoints here. */
		int i;
		WP *a=rethead();
		
		for(i=0;i<32;i++)
		{
			if(a==NULL) break; 
			bool sus;
			
			a->newvalue=expr(a->str, &sus);
			
			
			if(a->newvalue!=a->value)
			{
				printf("your watchpoint %s brokes\n",a->str);
				nemu_state=STOP;
				
			}
			
			
			if(a->next==NULL) break;

			a=a->next;
			
		}

#ifdef HAS_DEVICE
		extern void device_update();
		device_update();
#endif

		if(nemu_state != RUNNING) { return; }
	}

	if(nemu_state == RUNNING) { nemu_state = STOP; }
}
