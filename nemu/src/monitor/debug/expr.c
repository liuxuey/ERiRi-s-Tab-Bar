#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ = '=' , NUMBER = 'i',hex,reg,nottrue,a,yu,huo,muiply,gangling=114
	
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{" +",	NOTYPE},
	{"0x[0-9,a-f]{1,8}",hex},					//十六位
	{"\\$[a-z]{2,3}",reg},					//寄存器
	{"!=",nottrue},				//不等于
	{"!",muiply},				//阶乘运算
	{"\\&\\&",yu}	,					//与运算
	{"[||]{2}",huo},					//或运算
						
					// spaces
	{"\\+", '+'},					// plus
	{"\\-", '-'},					// minus
	{"\\*", '*'},					// mutiply
	{"\\/", '/'},					// divide
	{"[0-9]{1,10}", NUMBER},		// number
	{"\\(",'('},					// zuokuohao
	{"\\)",')'},					// youkuohao
	{"==", EQ},					// equal
	
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;	//token的类型
	char str[32];	//token里的数据
	int priority1; //token的优先级
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0; //tokens的个数

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				
				
				
				//char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;
				//Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				// 去除空格串
				if (rules[i].token_type == NOTYPE)
					continue;
				if (rules[i].token_type == 114)
					continue;
				

				switch(rules[i].token_type) {
					case 43: {tokens[nr_token].type=43; tokens[nr_token].priority1=1; nr_token++;  break;}   //case "+"
					case '-': {tokens[nr_token].type='-'; tokens[nr_token].priority1=1; nr_token++; break;}	//case "-"
					case '*': {tokens[nr_token].type='*'; tokens[nr_token].priority1=12; nr_token++; break;}
					case '/': {tokens[nr_token].type='/'; tokens[nr_token].priority1=12; nr_token++;  break;}
					case '(': {tokens[nr_token].type='('; tokens[nr_token].priority1=222; nr_token++; break;}
					case ')': {tokens[nr_token].type=')'; tokens[nr_token].priority1=222; nr_token++; break;}
					case EQ: {tokens[nr_token].type=EQ; tokens[nr_token].priority1=0; nr_token++; break;}
					case 'i': {
							    tokens[nr_token].type='i';
							   tokens[nr_token].priority1=10000000;
							   strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
							   nr_token++;
							   
							   
							    break;}
					case 106:	tokens[nr_token].type=106;
								tokens[nr_token].priority1=10000000;
								strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
								nr_token++;
								break;		// 		十六进制
					case 107:	tokens[nr_token].type=107;
								tokens[nr_token].priority1=10000000;
								strncpy(tokens[nr_token].str,&e[position-substr_len],substr_len);
								nr_token++;
								break;		//		寄存器
					case 108:	tokens[nr_token].type=108;
								tokens[nr_token].priority1=0;
								nr_token++;
								break;		//		！= 不等于
					case 110:	tokens[nr_token].type=110;
								tokens[nr_token].priority1=-4;
								nr_token++;
								break;		//		&& 与运算
					case 111:	tokens[nr_token].type=111;
								tokens[nr_token].priority1=-5;
								nr_token++;
								break;		//		||	或运算
					case 112:	tokens[nr_token].type=112;
								tokens[nr_token].priority1=13;
								nr_token++;
								break;		//		!	阶乘运算
					default: panic("please implement me");
				}
				if(tokens[0].type=='-')
				{
					tokens[0].type='m';
					
					tokens[0].priority1=1;
				}
				int case_minus;
				for(case_minus=0;case_minus<nr_token;case_minus++)
				{
					if(tokens[case_minus].type=='-')
					{
						if(tokens[case_minus-1].type!=')')
						{if(tokens[case_minus-1].type!='i')
							{
								if(tokens[case_minus-1].type!=106){
									if(tokens[case_minus-1].type!=107){
							
							tokens[case_minus].type='m'; 

							tokens[case_minus].priority1=13;
						}}}}
					}
				}
				if(tokens[0].type=='*')
				{	printf("i find a pointer!\n");
					tokens[0].type=113;
					
					tokens[0].priority1=13;
				}
				int case_pointer;
				for(case_pointer=0;case_pointer<nr_token;case_pointer++)
				{
					if(tokens[case_pointer].type=='*')
					{ 
						if(tokens[case_pointer-1].type!=')')
						{	
							if(tokens[case_pointer-1].type!='i')
							{
								if(tokens[case_pointer-1].type!=106)
								{if(tokens[case_pointer-1].type!=107){
							printf("i find a pointer!\n");
							tokens[case_pointer].type=113; 

							tokens[case_pointer].priority1=13;
						}}}}
					}
				}
				
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	
	nr_token--;
	return true; 
}
bool check_parenthese(int p,int q){		//找位置对不对括号的对应	
	int i;
	
	int left=0;//左括号的个数
	int right=0;//右括号的个数
	for(i=p;i<=q;i++)
	{
		if(tokens[i].type=='('||tokens[i].type==')')
		{
			if(tokens[i].type=='(')
			{
				left++;

			}
			else right++;
			if(right>left) return false;
		}
	}
	if(left!=right) return false; //左括号数！=右括号数
	return true;
}
bool check_parentheses(int p,int q){//最后的括号对应
	if(check_parenthese(p,q)) {
		if(tokens[p].type=='('&& tokens[q].type==')')
		return true;
	}
	return false;
}
	int finddominantoprator(int p,int q){		//找到拆分的运算符
		 //tokens的长度
		 //assert(check_parentheses(p,q),"表达式错误！");
		int i;
		int j=p;
		int flag=0;					//记录有没有左括号
		Token dominantop=tokens[p];	//从p位置开始找
		for(i=p;i<=q;i++)
		{
			if(tokens[i].type=='(') 	//读入左括号
			{
				flag=1;    				//将dominantop定位到左括号
			}
			else if(tokens[i].type==')')	//do op为左括号
			{
				flag=0;						//将do op定位到右括号
			}
			if(flag==0){if(dominantop.type!=112){
				if(dominantop.type!='m'){
					if(dominantop.type!=113){
			if(dominantop.priority1>=tokens[i].priority1)	//比较优先级
			{
				
				dominantop=tokens[i];
				j=i;						//do op的定位
			}}}}
			else {
				if(dominantop.priority1>tokens[i].priority1)	//比较优先级
			{
				
				dominantop=tokens[i];
				j=i;						//do op的定位
			}
			}
			
			}
		}
		return j;
	}
uint32_t sumbds(p,q)		//表达式求值
{
	//if(!check_parentheses(p,q))
	//	assert(0);
	if(p>q){// 不合法的情况
				
		assert(0);
	}
	else if(p==q)
	{
		int n=0;
		//printf("%s",tokens[p].str);
		if(tokens[p].type=='i'){
		sscanf(tokens[p].str,"%d",&n);}
		if(tokens[p].type==106)
		{
			sscanf(tokens[p].str,"%x",&n);
		}
		if(tokens[p].type==107)
		{
			if(!strcmp(tokens[p].str,"$eax"))	n=cpu.gpr[0]._32;
			if(!strcmp(tokens[p].str,"$ecx"))	n=cpu.gpr[1]._32;
			if(!strcmp(tokens[p].str,"$edx"))	n=cpu.gpr[2]._32;
			if(!strcmp(tokens[p].str,"$ebx"))	n=cpu.gpr[3]._32;
			if(!strcmp(tokens[p].str,"$esp"))	n=cpu.gpr[4]._32;
			if(!strcmp(tokens[p].str,"$ebp"))	n=cpu.gpr[5]._32;
			if(!strcmp(tokens[p].str,"$esi"))	n=cpu.gpr[6]._32;
			if(!strcmp(tokens[p].str,"$edi"))	n=cpu.gpr[7]._32;
			if(!strcmp(tokens[p].str,"$ax"))	n=cpu.gpr[0]._16;
			if(!strcmp(tokens[p].str,"$cx"))	n=cpu.gpr[1]._16;
			if(!strcmp(tokens[p].str,"$bx"))	n=cpu.gpr[2]._16;
			if(!strcmp(tokens[p].str,"$dx"))	n=cpu.gpr[3]._16;
			if(!strcmp(tokens[p].str,"$sp"))	n=cpu.gpr[4]._16;
			if(!strcmp(tokens[p].str,"$bp"))	n=cpu.gpr[5]._16;
			if(!strcmp(tokens[p].str,"$si"))	n=cpu.gpr[6]._16;
			if(!strcmp(tokens[p].str,"$di"))	n=cpu.gpr[7]._16;
			if(!strcmp(tokens[p].str,"$al"))	n=cpu.gpr[0]._8[0];
			if(!strcmp(tokens[p].str,"$ah"))	n=cpu.gpr[0]._8[1];
			if(!strcmp(tokens[p].str,"$cl"))	n=cpu.gpr[1]._8[0];
			if(!strcmp(tokens[p].str,"$ch"))	n=cpu.gpr[1]._8[1];
			if(!strcmp(tokens[p].str,"$dl"))	n=cpu.gpr[2]._8[0];
			if(!strcmp(tokens[p].str,"$dh"))	n=cpu.gpr[2]._8[1];
			if(!strcmp(tokens[p].str,"$bl"))	n=cpu.gpr[3]._8[0];
			if(!strcmp(tokens[p].str,"$bh"))	n=cpu.gpr[3]._8[1];
			if(!strcmp(tokens[p].str,"$eip"))	n=cpu.eip;

			
		}
		
		return n;
	}
	else if (check_parentheses(p,q))
	{
		
		return sumbds(p+1,q-1);
	}
	else 
	{	
		
		
		int op;
		
		op=finddominantoprator(p,q);
		
		 if(tokens[op].type==112)
		{
			
			uint32_t val=sumbds(op+1,q);
			if(val) return 0;
			else return 1;
		}
		if(tokens[op].type==113)
		{
			
			uint32_t val=sumbds(op+1,q);
			return swaddr_read (val,1);
		}
		else if(tokens[op].type=='m')
		{
			uint32_t val=sumbds(op+1,q);
			return (-1)*val;
		}
		 if(tokens[op].type==112)
		{
			
			uint32_t val=sumbds(op+1,q);
			if(val) return 0;
			else return 1;
		}
		else{
		
		uint32_t val1=sumbds(p,op-1);
		
		uint32_t val2=sumbds(op+1,q);
		
		switch (tokens[op].type)
		{
		case EQ:  if(val1==val2) return 1;
				  else return 0;
		case '+': return val1+val2;
		case '-': return val1-val2;
		case '*': return val1*val2;
		case '/': return val1/val2;
		

		case 108: if(val1==val2) return 0;
				  else return 1;
		case 110: if(val1&&val2) return 1;
				  else return 0;
		case 111: if(val1||val2) return 1;
				  else return 0;

		
		
		default: assert(0);}
			
		}
	}
	return 0;
}
uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}

	printf("%s\n", e);
	int i;
	int j;
	/* TODO: Insert codes to evaluate the expression. */
	uint32_t k=sumbds(0,nr_token);
	for(i=0;i<=nr_token;i++)		//清空前面存在的字符串
	{
		for(j=0;j<32;j++)
		{
			tokens[i].str[j]='\0';
		}
		
	}
	nr_token=0;
	return k;
	

	panic("please implement me");
	return 0;
}

