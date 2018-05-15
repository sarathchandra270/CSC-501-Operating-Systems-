/* wait.c - wait */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * wait  --  make current process wait on a semaphore
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;
SYSCALL	wait(int sem)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[26]=ctr1000;
        p->calls[26]=p->calls[26]+1;
        }
	STATWORD ps;    
	struct	sentry	*sptr;
	struct	pentry	*pptr;

	disable(ps);
	if (isbadsem(sem) || (sptr= &semaph[sem])->sstate==SFREE) {
		restore(ps);
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[26]=p->calls_time[26]+ctr1000-p->initial_time[26];
        	}
		return(SYSERR);
	}
	
	if (--(sptr->semcnt) < 0) {
		(pptr = &proctab[currpid])->pstate = PRWAIT;
		pptr->psem = sem;
		enqueue(currpid,sptr->sqtail);
		pptr->pwaitret = OK;
		resched();
		restore(ps);
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[26]=p->calls_time[26]+ctr1000-p->initial_time[26];
        	}
		return pptr->pwaitret;
	}
	restore(ps);
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[26]=p->calls_time[26]+ctr1000-p->initial_time[26];
        }
	return(OK);
}
