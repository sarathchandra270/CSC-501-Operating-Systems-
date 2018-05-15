#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>

unsigned long ctr1000;

int lock(int ldes1, int type, int priority)
{
	STATWORD ps;
	int tempptr,temp;
	int next;
	int pid;
	long curr_time;
	proctab[currpid].ltype = type;
	proctab[currpid].lprio = priority;
	//proctab[currpid].lwtime = ctr1000;
	
	disable(ps);
	if(isbadlock(ldes1) || lockarray[ldes1].lstate==LFREE)
	{
		restore(ps);
		return(SYSERR);
	}
	if(lockarray[ldes1].lstate==LUSED)
	{
		if(type==0)
		{
			lockarray[ldes1].lstate==LREAD;
			restore(ps);
			return(OK);
		}
		else if(type==1)
		{
			lockarray[ldes1].lstate==LWRITE;
			restore(ps);
			return(OK);
		}
	}
	if(lockarray[ldes1].lstate==LREAD)
	{
		if(type==0)
		{
			temp = lockarray[ldes1].lqtail;
			
			pid=getnode(temp);
			if(pid==-1 || proctab[pid].lprio < priority)
			{
				restore(ps);
				return(OK);
			}
			else if(proctab[pid].lprio > priority)
			{
				tempptr = lockarray[ldes1].lqhead;
				insert(currpid, tempptr, priority);
				restore(ps);
				return(OK);
			}
			else
			{
				curr_time = ctr1000-proctab[pid].lwtime;
				if(curr_time < 500)
				{
					restore(ps);
					return(OK);
				}
				else
				{
					tempptr = lockarray[ldes1].lqhead;
					insert(currpid, tempptr, priority);
					proctab[currpid].lwtime = ctr1000;
					
					restore(ps);
					return(OK);
				}
			}
			
		}
		else if(type==1)
		{
			//proctab[currpid].ltype = type;
			//proctab[currpid].lprio = priority;
			tempptr = lockarray[ldes1].lqhead;
			insert(currpid, tempptr, priority);
			proctab[currpid].lwtime = ctr1000;
			restore(ps);
			return(OK);
		}
	}
	if(lockarray[ldes1].lstate==LWRITE)
	{
		//proctab[currpid].ltype = type;
		//proctab[currpid].lprio = priority;
		tempptr = lockarray[ldes1].lqhead;
		insert(currpid, tempptr, priority);
		proctab[currpid].lwtime = ctr1000;
		restore(ps);
		return(OK);
	}
}

int insert(int proc, int head, int key)
{
	int	next;			/* runs through list		*/
	int	prev;

	next = q[head].qnext;
	while (proctab[q[next]].lprio < key)	/* tail has maxint as key	*/
		next = q[next].qnext;
	q[proc].qnext = next;
	q[proc].qprev = prev = q[next].qprev;
	//q[proc].qkey  = key;
	q[prev].qnext = proc;
	q[next].qprev = proc;
	return(OK);
}

int getnode(int next)
{
	if(q[next].qprev>NPROC-1)
		return -1;
	else
	{
		return q[next].qprev;
	}
}