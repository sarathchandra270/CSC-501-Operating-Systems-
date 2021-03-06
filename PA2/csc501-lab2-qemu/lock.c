#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>

unsigned long ctr1000;
int getnode1(int next);
void inherit(int ldes1,int priority);

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
		//kprintf("\n NEWEEEEEE The value of lock: %d", lockarray[ldes1].lstate);
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
			//proctab[currpid].plstate[ldes1]=lockarray[ldes1].lstate;
			//kprintf("\nThe value of lock: %d", lockarray[ldes1].lstate);
			restore(ps);
			return(OK);
		}
		else if(type==WRITE)
		{
			//kprintf("\nThe value of lock: %d", lockarray[ldes1].lstate);
			lockarray[ldes1].lstate=LWRITE;
			proctab[currpid].lhold[ldes1]=1;
			//proctab[currpid].plstate[ldes1]=lockarray[ldes1].lstate;
			restore(ps);
			return(OK);
		}
	}
	if(lockarray[ldes1].lstate==LREAD)
	{
		if(type==READ)
		{
			//printf("%d",currpid);
			temp = lockarray[ldes1].lqtail;
			//kprintf("\nThe value of lock AT WEREEKJDJFN: %d", lockarray[ldes1].lstate);
			pid=getnode1(temp);
			if(pid==-1 || proctab[pid].lprio < priority)
			{
				lockarray[ldes1].n_readers++;
				proctab[currpid].lhold[ldes1]=1;
				//kprintf("\nThe value of lock: %d", lockarray[ldes1].lstate);
				//proctab[currpid].plstate[ldes1]=lockarray[ldes1].lstate;
				restore(ps);
				return(OK);
			}
			else if(proctab[pid].lprio > priority)
			{
				//printf("coming here");
				
				tempptr = lockarray[ldes1].lqhead;
				proctab[currpid].pstate = PRWAIT;
				insert(currpid, tempptr, priority);
				inherit(ldes1,priority);
				proctab[currpid].lwtime = ctr1000;
				resched();
				restore(ps);
				return(OK);
			}
			else
			{
				curr_time = ctr1000-proctab[pid].lwtime;
				if(curr_time < 500)
				{
					lockarray[ldes1].n_readers++;
					proctab[currpid].lhold[ldes1]=1;
					restore(ps);
					return(OK);
				}
				else
				{
					tempptr = lockarray[ldes1].lqhead;
					proctab[currpid].pstate = PRWAIT;
					insert(currpid, tempptr, priority);
					inherit(ldes1,priority);
					proctab[currpid].lwtime = ctr1000;
					resched();
					restore(ps);
					return(OK);
				}
			}
			
		}
		else if(type==WRITE)
		{
			//kprintf("\nThe value of lock: %d", lockarray[ldes1].lstate);
			tempptr = lockarray[ldes1].lqhead;
			proctab[currpid].pstate = PRWAIT;
			insert(currpid, tempptr, priority);
			inherit(ldes1,priority);
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
		insert(currpid, tempptr, priority);
		inherit(ldes1,priority);
		proctab[currpid].lwtime = ctr1000;
		resched();
		restore(ps);
		return(OK);
	}
}
/*
int insert(int proc, int head, int key)
{
	int	next;			
	int	prev;

	next = q[head].qnext;
	while (proctab[q[next]].lprio < key)	
		next = q[next].qnext;
	q[proc].qnext = next;
	q[proc].qprev = prev = q[next].qprev;
	//q[proc].qkey  = key;
	q[prev].qnext = proc;
	q[next].qprev = proc;
	return(OK);
}
*/
int getnode1(int next)
{
	if(q[next].qprev>NPROC-1)
		return -1;
	else
	{
		return q[next].qprev;
	}
}

void inherit(int ldes1,int priority)
{
	if(lockarray[ldes1].maxprio<priority)
	{
		lockarray[ldes1].maxprio = priority;
		for(i=0;i<NPROC;i++)
		{
			if(proctab[i].lhold[ldes1]==1)
			{
				if(proctab[i].newprio<maxprio)
				{
					proctab[i].pinh = (proctab[i].pinh==0)?proctab[i].newprio:proctab[i].pinh;
					proctab[i].newprio=lockarray[ldes1].maxprio;
				}
			}
		}
	}
}