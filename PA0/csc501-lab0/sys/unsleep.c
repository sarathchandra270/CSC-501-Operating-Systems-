/* unsleep.c - unsleep */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sleep.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * unsleep  --  remove  process from the sleep queue prematurely
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;
SYSCALL	unsleep(int pid)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[25]=ctr1000;
        p->calls[25]=p->calls[25]+1;
        }
	STATWORD ps;    
	struct	pentry	*pptr;
	struct	qent	*qptr;
	int	remain;
	int	next;

        disable(ps);
	if (isbadpid(pid) ||
	    ( (pptr = &proctab[pid])->pstate != PRSLEEP &&
	     pptr->pstate != PRTRECV) ) {
		restore(ps);
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[25]=p->calls_time[25]+ctr1000-p->initial_time[25];
        	}
		return(SYSERR);
	}
	qptr = &q[pid];
	remain = qptr->qkey;
	if ( (next=qptr->qnext) < NPROC)
		q[next].qkey += remain;
	dequeue(pid);
	if ( (next=q[clockq].qnext) < NPROC)
		sltop = (int *) & q[next].qkey;
	else
		slnempty = FALSE;
        restore(ps);
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[25]=p->calls_time[25]+ctr1000-p->initial_time[25];
        }
	return(OK);
}
