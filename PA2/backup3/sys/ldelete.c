#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>

int ldelete(int lockdescriptor)
{
	
	STATWORD ps;    
	int	pid;
	struct	lentry	*lptr;

	disable(ps);
	if (isbadlock(lockdescriptor) || lockarray[lockdescriptor].lstate==LFREE) {
		restore(ps);
		return(SYSERR);
	}
	
	lptr = &lockarray[lockdescriptor];
	lptr->lstate = LFREE;
	lptr->n_readers=0;
	if (nonempty(lptr->lqhead)) {
		while( (pid=getfirst(lptr->lqhead)) != EMPTY)
		  {
		    proctab[pid].pwaitret = DELETED;
		    ready(pid,RESCHNO);
		  }
		resched();
	}
	restore(ps);
	return(OK);
}