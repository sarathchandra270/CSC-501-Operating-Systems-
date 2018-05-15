#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>
int getnode(int next);

SYSCALL releaseall(int numlocks, int ldes1, ...)
{
	unsigned long	*a;
	STATWORD 	ps;
	int i,flag=0;
	int pid;
	disable(ps);
	a = (unsigned long *)(&ldes1);
	for(i=0;i<numlocks;i++)
	{
		int lock=*(a++);
		if(lockarray[lock].lstate==LFREE)
		{
			flag=1;
		}
		else if(lockarray[lock].lstate==LUSED)
		{
			;
		}
		else if(lockarray[lock].lstate==LREAD)
		{
			lockarray[lock].n_readers--;
			if(lockarray[lock].n_readers>0)
			{
				;
			}
			else
			{
				if(getnode(lockarray[lock].lqtail==-1)
				{
					lockarray[lock].lstate=LUSED;
				}
				else
				{
					pid=getnode(lockarray[lock].lqtail);
					proctab[pid].plstate[lock]=LWRITE;
					lockarray[lock].lstate=LWRITE;
					ready(getlast(lockarray[lock].lqtail),RESCHNO);
				}
			}
		}
		else if(lockarray[lock].lstate==LWRITE)
		{
			pid=getnode(lockarray[lock].lqtail);
			if(proctab[pid].plstate[lock]==LREAD)
			{
				int write_flag=0;
				int first_w_prio;
				long first_w_time;
				long read_w_time;
				int ptr=q[lockarray[lock].lqtail].qprev;
				while(ptr!=lockarray[lock].lqhead)
				{
					if(write_flag==0)
					{
						if(proctab[ptr].plstate[lock]==LREAD)
						{
							ready(getlast(lockarray[lock].lqtail),RESCHNO);
						}
						else 
						{
							write_flag	=1;
							first_w_prio=q[getnode(lockarray[lock].lqtail)].qkey;
							first_w_time=proctab[q[getnode(lockarray[lock].lqtail)].qprev].lwtime;
						}
					}
					else
					{
						if(proctab[ptr].plstate[lock]==LREAD &&q[getnode(lockarray[lock].lqtail)].qkey==first_w_prio)
						{
							read_w_time=proctab[q[getnode(lockarray[lock].lqtail)].qprev].lwtime;
							if(read_w_time-first_w_time <500)
							{
								ready(getlast(lockarray[lock].lqtail),RESCHNO);
							}
						}
						else
						{
							;
						}
					}
					
					ptr=q[ptr].qprev;
				}
			}
			else
			{
				int w_write_flag=0;
				int w_first_w_prio;
				long w_first_w_time;
				long w_read_w_time;
				int ptr=q[lockarray[lock].lqtail].qprev;
				while(ptr!=lockarray[lock].lqhead)
				{
					if(w_write_flag==0)
					{
						if(proctab[ptr].plstate[lock]==LWRITE)
						{
							w_write_flag	=1;
							w_first_w_prio=q[getnode(lockarray[lock].lqtail)].qkey;
							w_first_w_time=proctab[q[getnode(lockarray[lock].lqtail)].qprev].lwtime;
						}
						else
						{
							;
						}
					}
					else
					{
						if(q[ptr].qkey<w_first_w_prio)
						{
							ready(getlast(lockarray[lock].lqtail),RESCHNO);
						}
						else if(q[ptr].qkey==w_first_w_prio && proctab[ptr].plstate[lock]==LWRITE)  //code doubt here 
						{
							;
						}
						else if(q[ptr].qkey==w_first_w_prio && proctab[ptr].plstate[lock]==LREAD)
						{
							w_read_w_time =proctab[q[ptr].qprev].lwtime;
							if(w_read_w_time-w_first_w_time <500)
							{
								lockarray[lock].n_readers++;
								ready(dequeue(ptr), RESCHNO);
							}
						}
					}
					ptr=q[ptr].qprev;
				}
				
			}
		}
		
	}
	resched();
	restore(ps);
	if(flag==1)
		return(SYSERR);
	else
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