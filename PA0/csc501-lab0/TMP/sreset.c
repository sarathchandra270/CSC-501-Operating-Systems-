/* sreset.c - sreset */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  sreset  --  reset the count and queue of a semaphore
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;
SYSCALL sreset(int sem, int count)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[22]=ctr1000;
        p->calls[22]=p->calls[22]+1;
        }
	STATWORD ps;    
	struct	sentry	*sptr;
	int	pid;
	int	slist;

	disable(ps);
	if (isbadsem(sem) || count<0 || semaph[sem].sstate==SFREE) {
		restore(ps);
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[22]=p->calls_time[22]+ctr1000-p->initial_time[22];
        	}
		return(SYSERR);
	}
	sptr = &semaph[sem];
	slist = sptr->sqhead;
	while ((pid=getfirst(slist)) != EMPTY)
		ready(pid,RESCHNO);
	sptr->semcnt = count;
	resched();
	restore(ps);
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[22]=p->calls_time[22]+ctr1000-p->initial_time[22];
        }
	return(OK);
}
