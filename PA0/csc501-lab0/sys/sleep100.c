/* sleep100.c - sleep100 */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * sleep100  --  delay the caller for a time specified in 1/100 of seconds
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;
SYSCALL sleep100(int n)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[20]=ctr1000;
        p->calls[20]=p->calls[20]+1;
        }
	STATWORD ps;    

	if (n < 0  || clkruns==0)
	        {
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[20]=p->calls_time[20]+ctr1000-p->initial_time[20];
        	} 
		return(SYSERR);
		}
	disable(ps);
	if (n == 0) {		/* sleep100(0) -> end time slice */
	        ;
	} else {
		insertd(currpid,clockq,n*10);
		slnempty = TRUE;
		sltop = &q[q[clockq].qnext].qkey;
		proctab[currpid].pstate = PRSLEEP;
	}
	resched();
        restore(ps);
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[20]=p->calls_time[20]+ctr1000-p->initial_time[20];
        }
	return(OK);
}
