#include <stdio.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

int cal_goodness(int ppid)
{
	int goodness;
	struct pentry *p;
	p=&proctab[ppid];
	if(p->pstate!=PRFREE)
	{
		if(p->counter<=0)
		{
			goodness=0;
		}
		else
		{
			goodness=p->pprio + p->counter;
		}
	}
	return goodness;
}
