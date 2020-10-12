#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	int flag;
	struct watchpoint *next;
	char str[64];
	int value;
	int newvalue;
	int duan;
	/* TODO: Add more members if necessary */


} WP;
WP *retwppool();
WP *rethead();
#endif
