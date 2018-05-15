/* suspend.c - suspend */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  suspend  --  suspend a process, placing it in hibernation
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;
SYSCALL	suspend(int pid)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[24]=ctr1000;
        p->calls[24]=p->calls[24]+1;
        }
	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority returned		*/

	disable(ps);
	if (isbadpid(pid) || pid==NULLPROC ||
	 ((pptr= &proctab[pid])->pstate!=PRCURR && pptr->pstate!=PRREADY)) {
		restore(ps);
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[24]=p->calls_time[24]+ctr1000-p->initial_time[24];
        	}
		return(SYSERR);
	}
	if (pptr->pstate == PRREADY) {
		pptr->pstate = PRSUSP;
		dequeue(pid);
	}
	else {
		pptr->pstate = PRSUSP;
		resched();
	}
	prio = pptr->pprio;
	restore(ps);
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[24]=p->calls_time[24]+ctr1000-p->initial_time[24];
        }
	return(prio);
}
