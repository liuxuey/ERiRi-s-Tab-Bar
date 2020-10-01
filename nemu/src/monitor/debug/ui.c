#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"
#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint32_t);
void new_wp(char * args, uint32_t n);
/* We use the `readline' library to provide more flexibility to read from stdin. */
char* rl_gets() {
	static char *line_read = NULL;

	if (line_read) {
		free(line_read);
		line_read = NULL;
	}

	line_read = readline("(nemu) ");

	if (line_read && *line_read) {
		add_history(line_read);
	}

	return line_read;
}

static int cmd_c(char *args) {
	cpu_exec(-1);
	return 0;
}

static int cmd_q(char *args) {
	return -1;
}
static int cmd_si(char *args)
{
	char *arg = strtok(NULL, " ");
	int n=0;
	if(arg==NULL) n=1;
	else sscanf(arg,"%d",&n);
	cpu_exec(n);
	return 0;
}
static int cmd_info(char *args) {
	char *arg = strtok(NULL, " ");
	if(*arg=='r') 
	{printf("eax : %d \n",cpu.gpr[0]._32);
	 printf("ecx : %d \n",cpu.gpr[1]._32);
	 printf("edx : %d \n",cpu.gpr[2]._32);
	 printf("ebx : %d \n",cpu.gpr[3]._32);
	 printf("esp : %d \n",cpu.gpr[4]._32);
	 printf("ebp : %d \n",cpu.gpr[5]._32);
	 printf("esi : %d \n",cpu.gpr[6]._32);
	 printf("edi : %d \n",cpu.gpr[7]._32);
	 printf("ax : %d \n",cpu.gpr[0]._16);
	 printf("cx : %d \n",cpu.gpr[1]._16);
	 printf("dx : %d \n",cpu.gpr[2]._16);
	 printf("bx : %d \n",cpu.gpr[3]._16);
	 printf("sp : %d \n",cpu.gpr[4]._16);
	 printf("bp : %d \n",cpu.gpr[5]._16);
	 printf("si : %d \n",cpu.gpr[6]._16);
	 printf("di : %d \n",cpu.gpr[7]._16);
	 printf("al : %d \n",cpu.gpr[0]._8[0]);
	 printf("ah : %d \n",cpu.gpr[0]._8[1]);
	 printf("cl : %d \n",cpu.gpr[1]._8[0]);
	 printf("ch : %d \n",cpu.gpr[1]._8[1]);
	 printf("dl : %d \n",cpu.gpr[2]._8[0]);
	 printf("dh : %d \n",cpu.gpr[2]._8[1]);
	 printf("bl : %d \n",cpu.gpr[3]._8[0]);
	 printf("bh : %d \n",cpu.gpr[3]._8[1]);
	 printf("eip : %d \n",cpu.eip);
	 
	}
	return 0;
} 

static int cmd_exam(char *args)//扫描内存
	{
	int i;
	int j;
	int add;
	char *arg=strtok(args," ");
	int n=atoi(arg);
	char *arg1 =strtok(NULL," ");
	sscanf(arg1,"%x",&add);
	for(i=0;i<n;i++)
	{
	uint32_t data =swaddr_read(add+i*4,4);
	printf("0x%08x ",add+i*4);
	for(j=0;j<4;j++){
	printf("0x%02x ",data&0xff);
	data =data >>8;}
	printf("\n");
	}
	return 0;
	}
static int cmd_cmpter(char* args)
{
	
	bool sus;
	printf("%d\n",expr(args, &sus));
	
	return 0;
}
static int cmd_watch(char* args)
{
	bool sus;
	uint32_t n=expr(args, &sus);
	new_wp(args,n);
	return 0;
}
static int cmd_help(char *args);

static struct {
	char *name;
	char *description;
	int (*handler) (char *);
} cmd_table [] = {
	{ "help", "Display informations about all supported commands", cmd_help },
	{ "c", "Continue the execution of the program", cmd_c },
	{ "q", "Exit NEMU", cmd_q },
	{"si","程序单步执行 N 条指令后暂停,当 N 没有给出时, 缺省为 1。",cmd_si},
	{"info","print the state of register ",cmd_info},
	{"x","扫描内存",cmd_exam},
	{"p","表达式求值",cmd_cmpter},
	{"w","watchpoint",cmd_watch}
	/* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
	/* extract the first argument */
	char *arg = strtok(NULL, " ");
	int i;

	if(arg == NULL) {
		/* no argument given */
		for(i = 0; i < NR_CMD; i ++) {
			printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
		}
	}
	else {
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(arg, cmd_table[i].name) == 0) {
				printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
				return 0;
			}
		}
		printf("Unknown command '%s'\n", arg);
	}
	return 0;
}

void ui_mainloop() {
	while(1) {
		char *str = rl_gets();
		char *str_end = str + strlen(str);

		/* extract the first token as the command */
		char *cmd = strtok(str, " ");
		if(cmd == NULL) { continue; }

		/* treat the remaining string as the arguments,
		 * which may need further parsing
		 */
		char *args = cmd + strlen(cmd) + 1;
		if(args >= str_end) {
			args = NULL;
		}

#ifdef HAS_DEVICE
		extern void sdl_clear_event_queue(void);
		sdl_clear_event_queue();
#endif

		int i;
		for(i = 0; i < NR_CMD; i ++) {
			if(strcmp(cmd, cmd_table[i].name) == 0) {
				if(cmd_table[i].handler(args) < 0) { return; }
				break;
			}
		}

		if(i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
	}
}
