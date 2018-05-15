/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;

SYSCALL getprio(int pid)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[3]=ctr1000;
        p->calls[3]=p->calls[3]+1;
        }
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[3]=p->calls_time[3]+ctr1000-p->initial_time[3];
        	}
		return(SYSERR);
	}
	restore(ps);
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[3]=p->calls_time[3]+ctr1000-p->initial_time[3];
        }
	return(pptr->pprio);
}
