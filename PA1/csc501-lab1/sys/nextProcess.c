#include <stdio.h>
#include <kernel.h>
#include <q.h>
#include <proc.h>

int nextProcess()
{
	int i,goodness_max=0;
	int next_process=0;
	struct pentry *p;
	for(i=0;i<NPROC;i++)
	{
		p=&proctab[i];
		if(p->pstate==PRREADY && p->goodness > goodness_max && p->quantum>0)
		{
			goodness_max = p->goodness;
			next_process = i;
		}
	}
return next_process;
}
