#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>
int getnode(int next);

int releaseall(int numlocks, int ldes1, ...)
{
	//printf("hello this is c");
	unsigned int *a;
	STATWORD 	ps;
	int i,flag=0;
	int pid;
	int d_array[NLOCKS];
	int k=0;
	disable(ps);
	a = (unsigned int *)(&ldes1);
	//kprintf("\nThe value of lock: %d", lockarray[*(a++)].lstate);
	for(i=0;i<numlocks;i++)
	{
		int lock=*(a++);
		if(lockarray[lock].lstate==LFREE)
		{
			flag=1;
		}
		else if(lockarray[lock].lstate==LUSED)
		{
			flag=1;
		}
		else if(lockarray[lock].lstate==LREAD)
		{
			//printf("heloo here");
			lockarray[lock].n_readers--;
			if(lockarray[lock].n_readers>0)
			{
				;
			}
			else
			{
				//pid=getnode(lockarray[lock].lqtail);
				if((getnode(lockarray[lock].lqtail))==-1)
				{
					lockarray[lock].lstate=LUSED;
					//proctab[pid].plstate[lock]=LWRITE;
					//kprintf("\nThe value of lock coreect: %d", lockarray[lock].lstate);
				}
				else
				{
					//printf("hello here i am");
					pid=getnode(lockarray[lock].lqtail);
					proctab[pid].plstate[lock]=WRITE;
					lockarray[lock].lstate=LWRITE;
					ready(getlast(lockarray[lock].lqtail),RESCHNO);
				}
			}
		}
		else if(lockarray[lock].lstate==LWRITE)
		{
			pid=getnode(lockarray[lock].lqtail);
			//printf("\n Helllo this is read fuckinggggggggggggg%d",proctab[pid].plstate[lock]);
			//pid=getnode(lockarray[lock].lqtail);
			
			if(proctab[pid].plstate[lock]==READ)
			{
				//printf("Helllo this is read inside");
				int write_flag=0;
				int first_w_prio;
				long first_w_time;
				long read_w_time;
				int ptr=q[lockarray[lock].lqtail].qprev;
				while(ptr!=lockarray[lock].lqhead)
				{
					if(write_flag==0)
					{
						if(proctab[ptr].plstate[lock]==READ)
						{
							//printf("Helllo this is read");
							lockarray[lock].n_readers++;
							ptr=q[ptr].qprev;
							ready(getlast(lockarray[lock].lqtail),RESCHNO);
							//break;
						}
						
						else 
						{
							write_flag	=1;
							first_w_prio=q[getnode(lockarray[lock].lqtail)].qkey;
							first_w_time=proctab[q[getnode(lockarray[lock].lqtail)].qprev].lwtime;
							ptr=q[ptr].qprev;
						}
					}
					else
					{
						//printf("check here");
						if(proctab[ptr].plstate[lock]==READ &&q[getnode(lockarray[lock].lqtail)].qkey==first_w_prio)
						{
							read_w_time=proctab[q[getnode(lockarray[lock].lqtail)].qprev].lwtime;
							if(read_w_time-first_w_time <500)
							{
								lockarray[lock].n_readers++;
								ptr=q[ptr].qprev;
								ready(getlast(lockarray[lock].lqtail),RESCHNO);
							}
						}
						else
						{
							ptr=q[ptr].qprev;
						}
					}
					//printf("Helllo this is read");
					//ptr=q[ptr].qprev;
				}
			}
			else
			{
				//printf("Hello this is fuck");
				int w_write_flag=0;
				int w_first_w_prio;
				long w_first_w_time;
				long w_read_w_time;
				int write_flag=0;
				int ptr=q[lockarray[lock].lqtail].qprev;
				while(ptr!=lockarray[lock].lqhead)
				{
					//printf("Hello while");
					if(w_write_flag==0)
					{
						//printf("Hello while 1");
						if(proctab[ptr].plstate[lock]==WRITE)
						{
							//printf("Hello while 2");
							w_write_flag	=1;
							w_first_w_prio=q[getnode(lockarray[lock].lqtail)].qkey;
							w_first_w_time=proctab[q[getnode(lockarray[lock].lqtail)].qprev].lwtime;
							ptr=q[ptr].qprev;
						}
						else
						{
							//printf("Hello while 2");
							ptr=q[ptr].qprev;;
						}
					}
					else
					{
						//printf("Hello while 3");
						if(q[ptr].qkey<w_first_w_prio && write_flag==0)
						{
							//printf("Hello while 4");
							ptr=q[ptr].qprev;
							ready(getlast(lockarray[lock].lqtail),RESCHNO);
							break;
						}
						else if(q[ptr].qkey==w_first_w_prio && proctab[ptr].plstate[lock]==WRITE)  //code doubt here 
						{
							ptr=q[ptr].qprev;;
						}
						else if(q[ptr].qkey==w_first_w_prio && proctab[ptr].plstate[lock]==READ)
						{
							//w_read_w_time =proctab[q[ptr]].lwtime;
							w_read_w_time =proctab[q[ptr].qprev].lwtime;
							if(w_read_w_time-w_first_w_time <500)
							{
								write_flag=1;
								lockarray[lock].n_readers++;
								d_array[k++]=ptr;
								//ready(dequeue(ptr), RESCHNO);
							}
							else
							{
								/*if(k==0)
								{
									ready(getlast(lockarray[lock].lqtail),RESCHNO);
								}*/
								break;
							}
							ptr=q[ptr].qprev;
						}
					}
					//ptr=q[ptr].qprev;
				}
				int j;
				/*if(k==0)
				{
					ready(getlast(lockarray[lock].lqtail),RESCHNO);
				}*/
				for(j=0;j<k;j++)
				{
						ready(dequeue(d_array[j]), RESCHNO);
					
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