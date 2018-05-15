#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>

struct lentry lockarray[NLOCKS];
int nextlock;

void linit()
{
	int i,j;
	nextlock=NLOCKS-1;
	struct lentry *lptr;
	for(j=0; j<NPROC; j++)
	{
		proctab[j].lblocked = -1;
	}
	for (i=0 ; i<NLOCKS ; i++) 
	{
	      /* initialize locks */
                (lptr = &lockarray[i])->lstate = LFREE;
                lptr->lqtail = 1 + (lptr->lqhead = newqueue());
        }

}
