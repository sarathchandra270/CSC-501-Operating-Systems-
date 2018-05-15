/* recvtim.c - recvtim */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 *  recvtim  -  wait to receive a message or timeout and return result
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;

SYSCALL	recvtim(int maxwait)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[8]=ctr1000;
        p->calls[8]=p->calls[8]+1;
        }
	STATWORD ps;    
	struct	pentry	*pptr;
	int	msg;

	if (maxwait<0 || clkruns == 0)
		{
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[8]=p->calls_time[8]+ctr1000-p->initial_time[8];
        	}
		return(SYSERR);
		}
	disable(ps);
	pptr = &proctab[currpid];
	if ( !pptr->phasmsg ) {		/* if no message, wait		*/
	        insertd(currpid, clockq, maxwait*1000);
		slnempty = TRUE;
		sltop = (int *)&q[q[clockq].qnext].qkey;
	        pptr->pstate = PRTRECV;
		resched();
	}
	if ( pptr->phasmsg ) {
		msg = pptr->pmsg;	/* msg. arrived => retrieve it	*/
		pptr->phasmsg = FALSE;
	} else {			/* still no message => TIMEOUT	*/
		msg = TIMEOUT;
	}
	restore(ps);
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[8]=p->calls_time[8]+ctr1000-p->initial_time[8];
        }
	return(msg);
}
