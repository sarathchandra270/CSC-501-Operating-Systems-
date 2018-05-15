/* signal.c - signal */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * signal  --  signal a semaphore, releasing one waiting process
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;
SYSCALL signal(int sem)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[16]=ctr1000;
        p->calls[16]=p->calls[16]+1;
        }
	STATWORD ps;    
	register struct	sentry	*sptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[16]=p->calls_time[16]+ctr1000-p->initial_time[16];
        	}
		return(SYSERR);
	}
	if ((sptr->semcnt++) < 0)
		ready(getfirst(sptr->sqhead), RESCHYES);
	restore(ps);
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[16]=p->calls_time[16]+ctr1000-p->initial_time[16];
        }
	return(OK);
}
