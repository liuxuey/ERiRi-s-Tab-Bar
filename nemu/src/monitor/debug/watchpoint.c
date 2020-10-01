#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;
WP *retwppool()
{
	return wp_pool;
}
void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
		wp_pool[i].flag = 0;
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	
	
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

void new_wp(char * args, uint32_t n)
{
	if(free_==NULL)
	{
		assert(0);
	}
	WP* newwp=free_;
	while(newwp->next->next!=NULL)
	{
		newwp=newwp->next;
	}
	WP* newwp1=newwp->next;
	newwp->next=NULL;
	WP* head1=head;
	if(head1==NULL)
	{
		head=newwp1;
		newwp1->NO=1;
		newwp1->next=NULL;
		newwp1->flag=1;
		strcpy(newwp1->str,args);
		newwp1->value=n;
	}
	else 
	{while(head1->next!=NULL)
	{
		head1=head1->next;
	}
	printf("3\n");
	head1->next=newwp1;
	newwp1->NO=head1->NO+1;
	newwp1->next=NULL;
	newwp1->flag=1; 
	printf("2\n");
	strcpy(newwp1->str,args);
	newwp1->value=n;}
}
void free_wp(WP *wp)
{
	WP* head1=head;
	while(head1->next!=wp)
	{
		head1=head1->next;
	} 		//找到取出位置	
	WP* newwp=free_;
	while(newwp->next!=NULL)
	{
		newwp=newwp->next;
	}		//找到放置位置
	head1->next=wp->next;
	WP* wp1=wp->next;
	wp1->NO=head1->NO+1; //取出
	newwp->next=wp;
	wp->next=NULL;
	wp->NO=newwp->NO+1;
}


