/* sleep.c - sleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep  --  delay the calling process n seconds
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;


SYSCALL	sleep(int n)
{
	if(flag==1)
	{
	struct pentry *p=&proctab[currpid];
	p->initial_time[18]=ctr1000;
        p->calls[18]=p->calls[18]+1;
	}
	STATWORD ps;    
	if (n<0 || clkruns==0)
	{
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[18]=p->calls_time[18]+ctr1000-p->initial_time[18];
        	}
		return(SYSERR);
	}
	if (n == 0) {
	        disable(ps);
		resched();
		restore(ps);
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[18]=p->calls_time[18]+ctr1000-p->initial_time[18];
        	}
		return(OK);
	}
	while (n >= 1000) {
		sleep10(10000);
		n -= 1000;
	}
	if (n > 0)
		sleep10(10*n);
	if(flag==1)
	{
		struct pentry *p=&proctab[currpid];
		p->calls_time[18]=p->calls_time[18]+ctr1000-p->initial_time[18];
	}
	return(OK);
}
