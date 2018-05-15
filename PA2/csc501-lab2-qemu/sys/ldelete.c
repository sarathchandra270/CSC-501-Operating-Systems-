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
	int	pid, i;
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
	
	/*for (i=0;i<NPROC;i++){
		if (proctab[i].ret_del[lockdescriptor]==1){
			if (lockarray[lockdescriptor].del_wait[i]==1){
				proctab[i].ret_del[lockdescriptor]=-4;
				lockarray[lockdescriptor].del_wait[i]=0;
				dequeue(i);
				ready(i, RESCHNO);
				
			}
			else{
				proctab[i].ret_del[lockdescriptor] = -5;
			}
			
		}
		
	}*/
	//resched();
	restore(ps);
	return(OK);
}