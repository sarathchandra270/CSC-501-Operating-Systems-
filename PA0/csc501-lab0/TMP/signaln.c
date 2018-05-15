/* signaln.c - signaln */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  signaln -- signal a semaphore n times
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;
SYSCALL signaln(int sem, int count)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[17]=ctr1000;
        p->calls[17]=p->calls[17]+1;
        }
	STATWORD ps;    
	struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || semaph[sem].sstate==SFREE || count<=0) {
		restore(ps);
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[17]=p->calls_time[17]+ctr1000-p->initial_time[17];
        	}
		return(SYSERR);
	}
	sptr = &semaph[sem];
	for (; count > 0  ; count--)
		if ((sptr->semcnt++) < 0)
			ready(getfirst(sptr->sqhead), RESCHNO);
	resched();
	restore(ps);
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[17]=p->calls_time[17]+ctr1000-p->initial_time[17];
        }
	return(OK);
}
