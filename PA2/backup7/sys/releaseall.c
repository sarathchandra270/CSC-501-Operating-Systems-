#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>
int getnode(int next);
void update_lock(int lock, int pid);

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
		update_lock(lock, currpid);
		if(lockarray[lock].lstate==LFREE)
		{
			//printf("\nAre you free\n");
			flag=1;
		}
		else if(lockarray[lock].lstate==LUSED)
		{
			//printf("\nare you here\n");
			flag=1;
		}
		else if(lockarray[lock].lstate==LREAD)
		{
			
			lockarray[lock].n_readers--;
			//printf("heloo here %d",lockarray[lock].n_readers);
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
					proctab[currpid].lblocked=-1;
					lockarray[lock].plist[currpid]=0;
					proctab[currpid].lhold[lock]=0;
					//proctab[pid].plstate[lock]=LWRITE;
					//kprintf("\nThe value of lock coreect: %d", lockarray[lock].lstate);
				}
				else
				{
					//printf("hello here i am");
					pid=getnode(lockarray[lock].lqtail);
						proctab[pid].plstate[lock]=WRITE;
						lockarray[lock].lstate=LWRITE;
						proctab[currpid].lblocked=-1;
						lockarray[lock].plist[currpid]=0;
						proctab[currpid].lhold[lock]=0;
					ready(getlast(lockarray[lock].lqtail),RESCHNO);
				}
			}
		}
		else if(lockarray[lock].lstate==LWRITE)
		{
			pid=getnode(lockarray[lock].lqtail);
			
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
							//printf("BEFORE Helllo this is read %d",lockarray[lock].n_readers);
							lockarray[lock].n_readers++;
							//printf("\nAFTERHelllo this is read %d",lockarray[lock].n_readers);
							ptr=q[ptr].qprev;
							proctab[pid].plstate[lock]=READ;
							lockarray[lock].lstate=LREAD;
							proctab[currpid].lblocked=-1;
							lockarray[lock].plist[currpid]=0;
							proctab[currpid].lhold[lock]=0;
							ready(getlast(lockarray[lock].lqtail),RESCHNO);
							//break;
						}
						
						else 
						{
							//printf("why here\n");
							write_flag	=1;
							first_w_prio=q[getnode(lockarray[lock].lqtail)].qkey;
							//printf("%d",first_w_prio);
							first_w_time=proctab[q[getnode(lockarray[lock].lqtail)].qprev].lwtime;
							ptr=q[ptr].qprev;
						}
					}
					else
					{
						
						
						if(proctab[ptr].plstate[lock]==READ &&q[ptr].qkey==first_w_prio)
						{
							//printf("check here IF");
							read_w_time=proctab[q[getnode(lockarray[lock].lqtail)].qprev].lwtime;
							if(read_w_time-first_w_time <500)
							{
								lockarray[lock].n_readers++;
								ptr=q[ptr].qprev;
								proctab[pid].plstate[lock]=READ;
								lockarray[lock].lstate=LREAD;
								proctab[currpid].lblocked=-1;
								lockarray[lock].plist[currpid]=0;
								proctab[currpid].lhold[lock]=0;
								ready(getlast(lockarray[lock].lqtail),RESCHNO);
							}
							else
							{
								ptr=q[ptr].qprev;
							}
						}
						else
						{
							//printf("check here ELSE"); 
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
							proctab[pid].plstate[lock]=WRITE;
							lockarray[lock].lstate=LWRITE;
							proctab[currpid].lblocked=-1;
							lockarray[lock].plist[currpid]=0;
							proctab[currpid].lhold[lock]=0;
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
								proctab[currpid].lblocked=-1;
								lockarray[lock].plist[currpid]=0;
								proctab[currpid].lhold[lock]=0;
								d_array[k++]=ptr;
								//ready(dequeue(ptr), RESCHNO);
							}
							else
							{
								if(k==0)
								{
								proctab[pid].plstate[lock]=WRITE;
								lockarray[lock].lstate=LWRITE;
									proctab[currpid].lblocked=-1;
									lockarray[lock].plist[currpid]=0;
									proctab[currpid].lhold[lock]=0;
									ready(getlast(lockarray[lock].lqtail),RESCHNO);
								}
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
					proctab[pid].plstate[lock]=READ;
					lockarray[lock].lstate=LREAD;
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

void update_lock(int lock, int pid)
{
	int i,j;
	int max=0;
	proctab[pid].lhold[lock]=0;
		
	for(j=0;j<NPROC;j++)
	{
		if(proctab[pid].lhold[j]==1)
		{
			int temp = q[lockarray[j].lqtail].qprev;  /* CAN USE MAXPRIO IN LENTRY INSTEAD OF ITERATING LOCKARRAY*/
			while(temp!=lockarray[j].lqhead)
			{
				if(proctab[temp].pprio>max)
				{
					max= proctab[temp].pprio;
				}
				temp = q[temp].qprev;
			}
				
		}
	}
	if(proctab[pid].pinh==0)
	{
		if(proctab[pid].pprio<max)
		{
			proctab[pid].pinh = (proctab[pid].pinh==0)?proctab[pid].pprio:proctab[pid].pinh;
			proctab[pid].pprio=max;
		}
	}
	else
	{
		if(proctab[pid].pinh<max)
		{
			proctab[pid].pprio=max;
		}
		else
		{
			proctab[pid].pprio=proctab[pid].pinh;
			proctab[pid].pinh=0;
		}
	}
	//proctab[pid].pprio=max;	
}
