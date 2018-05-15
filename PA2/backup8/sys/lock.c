#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>

unsigned long ctr1000;
int getnode1(int next);
void inherit(int ldes1,int prio);

int lock(int ldes1, int type, int priority)
{
	STATWORD ps;
	int tempptr,temp;
	int next;
	int pid;
	long curr_time;
	
	disable(ps);
	
	if(isbadlock(ldes1) || lockarray[ldes1].lstate==LFREE)
	{
		restore(ps);
		return(SYSERR);
	}
	proctab[currpid].ltype = type;
	proctab[currpid].lprio = priority;
	proctab[currpid].plstate[ldes1]=type;
	if(lockarray[ldes1].lstate==LUSED)
	{
		if(type==READ)
		{
			lockarray[ldes1].n_readers++;
			lockarray[ldes1].lstate=LREAD;
			proctab[currpid].lhold[ldes1]=1;
			lockarray[ldes1].plist[currpid]=1;
			restore(ps);
			return(OK);
		}
		else if(type==WRITE)
		{
			lockarray[ldes1].lstate=LWRITE;
			proctab[currpid].lhold[ldes1]=1;
			lockarray[ldes1].plist[currpid]=1;
			restore(ps);
			return(OK);
		}
	}
	if(lockarray[ldes1].lstate==LREAD)
	{
		if(type==READ)
		{
			temp = lockarray[ldes1].lqtail;
			pid=getnode1(temp);
			if(pid==-1 || proctab[pid].lprio < priority)
			{
				lockarray[ldes1].n_readers++;
				proctab[currpid].lhold[ldes1]=1;
				lockarray[ldes1].plist[currpid]=1;
				inherit(ldes1,proctab[currpid].pprio);
				restore(ps);
				return(OK);
			}
			else if(proctab[pid].lprio > priority)
			{
				tempptr = lockarray[ldes1].lqhead;
				proctab[currpid].pstate = PRWAIT;
				proctab[currpid].lblocked = ldes1;
				//inherit(ldes1,proctab[currpid].pprio);
				insert(currpid, tempptr, priority);
				inherit(ldes1,proctab[currpid].pprio);
				proctab[currpid].lwtime = ctr1000;
				resched();
				restore(ps);
				return(OK);
			}
			else
			{
				/*curr_time = ctr1000-proctab[pid].lwtime;
				if(curr_time < 500)
				{*/
					lockarray[ldes1].n_readers++;
					proctab[currpid].lhold[ldes1]=1;
					lockarray[ldes1].plist[currpid]=1;
					inherit(ldes1,proctab[currpid].pprio);
					restore(ps);
					return(OK);
				/*}
				else
				{
					tempptr = lockarray[ldes1].lqhead;
					proctab[currpid].pstate = PRWAIT;
					inherit(ldes1,proctab[currpid].pprio);
					proctab[currpid].lblocked = ldes1;
					insert(currpid, tempptr, priority);
					proctab[currpid].lwtime = ctr1000;
					resched();
					restore(ps);
					return(OK);
				}*/
			}
			
		}
		else if(type==WRITE)
		{
			tempptr = lockarray[ldes1].lqhead;
			proctab[currpid].pstate = PRWAIT;
			proctab[currpid].lblocked = ldes1;
			//inherit(ldes1,proctab[currpid].pprio);
			insert(currpid, tempptr, priority);
			inherit(ldes1,proctab[currpid].pprio);
			proctab[currpid].lwtime = ctr1000;
			resched();
			restore(ps);
			return(OK);
		}
	}
	if(lockarray[ldes1].lstate==LWRITE)
	{
		tempptr = lockarray[ldes1].lqhead;
		proctab[currpid].pstate = PRWAIT;
		proctab[currpid].lblocked = ldes1;
		//inherit(ldes1,proctab[currpid].pprio);
		insert(currpid, tempptr, priority);
		inherit(ldes1,proctab[currpid].pprio);
		proctab[currpid].lwtime = ctr1000;
		resched();
		restore(ps);
		return(OK);
	}
}

int getnode1(int next)
{
	if(q[next].qprev>NPROC-1)
		return -1;
	else
	{
		return q[next].qprev;
	}
}

void inherit(int ldes1, int prio)
{
	int max=0;
	int temp = q[lockarray[ldes1].lqtail].qprev;
	while(temp!=lockarray[ldes1].lqhead)
	{
		if(proctab[temp].pprio>max)
		{
			max= proctab[temp].pprio;
		}
		temp = q[temp].qprev;
	}
	lockarray[ldes1].maxprio = max;
	int i;
	for(i=0;i<NLOCKS;i++)
		{
			if(lockarray[ldes1].plist[i]==1)
			{
				if(proctab[i].pinh==0)
				{
					if(proctab[i].pprio<max)
					{
					proctab[i].pinh = (proctab[i].pinh==0)?proctab[i].pprio:proctab[i].pinh;
					proctab[i].pprio=max;
					}
				}
				else
				{
					if(proctab[i].pinh<max)
					{
						proctab[i].pprio=max;
					}
					else
					{
						proctab[i].pprio=proctab[i].pinh;
						proctab[i].pinh=0;
					}
				}
			}
		}
}

/*void inherit(int ldes1,int prio)
{
	if(lockarray[ldes1].maxprio<prio)
	{
		printf("hello %d",lockarray[ldes1].maxprio); 
		lockarray[ldes1].maxprio = prio;
		int i;
		for(i=0;i<NLOCKS;i++)
		{
			if(lockarray[ldes1].plist[i]==1)
			{
				if(proctab[i].pprio<lockarray[ldes1].maxprio)
				{
					proctab[i].pinh = (proctab[i].pinh==0)?proctab[i].pprio:proctab[i].pinh;
					proctab[i].pprio=lockarray[ldes1].maxprio;
				}
			}
		}
		printf("hello %d",lockarray[ldes1].maxprio);
	}
}*/