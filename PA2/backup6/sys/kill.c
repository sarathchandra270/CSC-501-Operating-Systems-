/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>

void find_maxprio(int k_lock);
void update_max(int k_lock);
/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	
	semaph[pptr->psem].semcnt++;
	
	dequeue(pid);
	
	int k_lock = proctab[pid].lblocked;
	proctab[pid].lblocked = -1;
	lockarray[k_lock].plist[pid]=0;
	find_maxprio(k_lock);
	update_max(k_lock);

	case PRREADY:	dequeue(pid);
			pptr->pstate = PRFREE;
			break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:	pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}

void find_maxprio(int k_lock)
{
	int max=0;
	int temp = q[lockarray[k_lock].lqtail].qprev;
	//printf("Hello");
	while(temp!=lockarray[k_lock].lqhead)
	{
		if(proctab[temp].pprio>max)
		{
			max= proctab[temp].pprio;
		}
		temp = q[temp].qprev;
	}
	lockarray[k_lock].maxprio = max;
	//printf("\n%d",lockarray[k_lock].maxprio);
}
void update_max(int k_lock)
{
	int i;
	for(i=0;i<NPROC;i++)
	{
		if(lockarray[k_lock].plist[i]==1)
		{
			//printf("%d\n",i);
			//printf("%d",lockarray[k_lock].maxprio);
			if(proctab[i].pinh==0)
			{
				if(proctab[i].pprio<lockarray[k_lock].maxprio)
				{
					proctab[i].pinh = (proctab[i].pinh==0)?proctab[i].pprio:proctab[i].pinh;
					proctab[i].pprio=lockarray[k_lock].maxprio;
				}
			}
			else
			{
				//printf("HEllo MEOLLOWWW");
				if(proctab[i].pinh<lockarray[k_lock].maxprio)
				{
					proctab[i].pprio=lockarray[k_lock].maxprio;
				}
				else
				{
					proctab[i].pprio=proctab[i].pinh;
				}
			}
		}
	}
}
