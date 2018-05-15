/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
 void update_chprio(int pid,int newprio);
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;
	update_chprio(pid,newprio);	
	restore(ps);
	return(newprio);
}

void update_chprio(int pid,int newprio)
{
	int j;
	for(j=0;j<NPROC;j++)
	{
		if(proctab[pid].lhold[j]==1)
		{
			if(lockarray[j].maxprio>proctab[pid].pprio)
			{
				proctab[pid].pprio=lockarray[j].maxprio;
				proctab[pid].pinh=newprio;
			}
		}
	}
	if(proctab[pid].pprio==newprio)
	{
		proctab[pid].pinh=0;
	}
	else
	{
		
		proctab[pid].pinh=newprio;
	}
	if(proctab[pid].lblocked!=0)
	{
		find_maxprio(proctab[pid].lblocked);
		update_max(proctab[pid].lblocked);
	}

}
