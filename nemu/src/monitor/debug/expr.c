#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ = '=' , NUMBER = 'i',hex,reg,nottrue,a,yu,huo,muiply
	
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */
	{"0x[0-9,a-f]{1,8}",hex},					//十六位
	//{"\$[a-z]{2,3}",reg},					//寄存器
	{"[!=]",nottrue},				//不等于
	{"[&&]",yu}	,					//与运算
	{"[||]",huo},					//或运算
	{"[!]",muiply},				//阶乘运算
						
	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus
	{"\\-", '-'},					// minus
	{"\\*", '*'},					// mutiply
	{"\\/", '/'},					// divide
	{"[0-9]{1,10}", NUMBER},		// number
	{"\\(",'('},					// zuokuohao
	{"\\)",')'},					// youkuohao
	{"==", EQ}						// equal
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
				
				
				
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;
				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */
				// 去除空格串
				if (rules[i].token_type == NOTYPE)
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
								break;		//		！=
					case 110:	tokens[nr_token].type=110;
								tokens[nr_token].priority1=0;

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
						if(tokens[case_minus-1].type!=(')'||'i'))
						{
							tokens[case_minus].type='m'; 
							tokens[case_minus].priority1=100;
						}
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
			if(flag==0){
			if(dominantop.priority1>=tokens[i].priority1)	//比较优先级
			{
				
				dominantop=tokens[i];
				j=i;						//do op的定位
			}}
		}
		return j;
	}
uint32_t sumbds(p,q)		//表达式求值
{
	//if(!check_parentheses(p,q))
	//	assert(0);
	if(p>q){
				//printf("%d,   %d\n",p,q);
		assert(0);
	}
	else if(p==q)
	{
		int n=0;
		//printf("%s",tokens[p].str);
		sscanf(tokens[p].str,"%d",&n);
		
		return n;
	}
	else if (check_parentheses(p,q))
	{
		
		return sumbds(p+1,q-1);
	}
	else 
	{
		if(tokens[p].type=='m')
		{
			return sumbds(p+1,q)*(-1);
		}//出现负号的情况
		int op;
		
		op=finddominantoprator(p,q);
		
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
		
		
		default: assert(0);
			
		}
	}
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

