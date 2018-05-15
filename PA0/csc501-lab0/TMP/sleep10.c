/* sleep10.c - sleep10 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep10  --  delay the caller for a time specified in tenths of seconds
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;
SYSCALL	sleep10(int n)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[19]=ctr1000;
        p->calls[19]=p->calls[19]+1;
        }
	STATWORD ps;    
	if (n < 0  || clkruns==0)
	        { 
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[19]=p->calls_time[19]+ctr1000-p->initial_time[19];
        	}
		return(SYSERR);
		}
	disable(ps);
	if (n == 0) {		/* sleep10(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*100);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[19]=p->calls_time[19]+ctr1000-p->initial_time[19];
        }
	return(OK);
}
