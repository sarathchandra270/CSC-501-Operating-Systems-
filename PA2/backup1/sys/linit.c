#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>


void linit()
{
	int i;
	struct lentry *lptr;
	for (i=0 ; i<NLOCKS ; i++) 
	{
	      /* initialize locks */
                (lptr = &lockarray[i])->lstate = LFREE;
                lptr->lqtail = 1 + (lptr->lqhead = newqueue());
        }

}
