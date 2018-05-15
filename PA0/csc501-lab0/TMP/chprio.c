/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;

SYSCALL chprio(int pid, int newprio)
{
	if(flag==1)
    {
        struct pentry *p=&proctab[currpid];
        p->initial_time[1]=ctr1000;
        p->calls[1]=p->calls[1]+1;
    }
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[1]=p->calls_time[1]+ctr1000-p->initial_time[1];
        	}
		return(SYSERR);
	}
	pptr->pprio = newprio;
	restore(ps);
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[1]=p->calls_time[1]+ctr1000-p->initial_time[1];
        }
	return(newprio);
}
