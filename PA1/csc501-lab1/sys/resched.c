/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lab1.h>

static int choice=0;
int epoch=0;
void setschedclass(int sched_class);
int getschedclass();
int nextProcess();

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state fo *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */

	if(getschedclass()==1)
	{
		int pri_sum=0;
        	int r_num=0;
		int tempptr;
		optr= &proctab[currpid];
                if (optr->pstate == PRCURR)
                {
                optr->pstate = PRREADY;
                insert(currpid,rdyhead,optr->pprio);
                }
		tempptr = q[rdytail].qprev;
		while(tempptr!=rdyhead)
                {
                pri_sum +=q[tempptr].qkey;
                tempptr=q[tempptr].qprev;
                }
                tempptr=q[rdytail].qprev;
                if(pri_sum==0)
                {
			tempptr=0;
                }
                else
                {
                        r_num=rand() % pri_sum;
		
                        while(r_num>q[tempptr].qkey&&tempptr!=rdyhead)
                        {
                        r_num -=q[tempptr].qkey;
                        tempptr=q[tempptr].qprev;
                        }
                }
		nptr = &proctab[ tempptr];
		currpid=tempptr;
		dequeue(tempptr);
                nptr->pstate = PRCURR;          /* mark it currently running    */

#ifdef  RTCLOCK
	preempt = QUANTUM;              /* reset preemption counter     */
#endif

                ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

                /* The OLD process returns here when resumed. */
                return(OK);
	}
	else if(getschedclass()==2)
	{
		
	int next_pid;
	optr=&proctab[currpid];
	
	epoch = epoch -(optr->quantum-preempt);

                
	if (optr->pstate == PRCURR) {
                optr->pstate = PRREADY;
                insert(currpid,rdyhead,optr->pprio);
        }
	
		
	if(epoch==0)
	{
		int i;
		for(i=1;i<NPROC;i++)
		{
			struct pentry *p=&proctab[i];
			p->prio_copy = p->pprio;
			if(p->pstate!=PRFREE)
			{
			p->quantum = (p->counter/2)+p->prio_copy;
			p->counter = p->quantum;
			p->goodness = p->counter + p->prio_copy;
			epoch = epoch + p->quantum;
			}
		}

		next_pid = nextProcess();

                	nptr=&proctab[next_pid];
                	
                	nptr=&proctab[currpid=dequeue(next_pid)];
                	nptr->pstate=PRCURR;
                	#ifdef  RTCLOCK
                		preempt = nptr->counter;              /* reset preemption counter     */
                	#endif
                	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                
          /* The OLD process returns here when resumed. */
                	return OK;
       
	}
	else if(epoch!=0)
	{

		optr->goodness = optr->goodness-( optr->quantum-preempt);

		if(preempt<0)
		{
		optr->quantum=0;
		}
		else
		{
		optr->quantum=preempt;
		}
			
	        optr->counter = optr->quantum;
        
        	if(optr->counter ==0)
        	{
                	optr->goodness=0;
        	}
		
		next_pid = nextProcess();
		optr=&proctab[currpid];
               	nptr=&proctab[next_pid];
                
                nptr=&proctab[currpid=dequeue(next_pid)];
                nptr->pstate=PRCURR;
                #ifdef  RTCLOCK
             		preempt = nptr->counter;              /* reset preemption counter     */
               	
		#endif
                ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
                          /* The OLD process returns here when resumed. */
               	return OK;			
	}

}
	else
	{
		if ( ( (optr= &proctab[currpid])->pstate == PRCURR) && (lastkey(rdytail)<optr->pprio))
		{
                	return(OK);
        	}

        /* force context switch */

        	if (optr->pstate == PRCURR) 
		{
                	optr->pstate = PRREADY;
                	insert(currpid,rdyhead,optr->pprio);
        	}

        /* remove highest priority process at end of ready list */

        	nptr = &proctab[ (currpid = getlast(rdytail)) ];
        	nptr->pstate = PRCURR;          /* mark it currently running    */   
	#ifdef  RTCLOCK
        	preempt = QUANTUM;              /* reset preemption counter     */
	#endif

        	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);

        /* The OLD process returns here when resumed. */
        	return(OK);

	}
}
void setschedclass(int sched_class)
{

	choice=sched_class;
}

int getschedclass()
{
	return choice;
}
int nextProcess()
{
        int i,goodness_max=0;
        int next_process=0;
	for(i=q[rdytail].qprev;i!=rdyhead;i=q[i].qprev)
	{
		if(goodness_max<proctab[i].goodness)
		{
			goodness_max = proctab[i].goodness;
			next_process=i;
		}
	}
return next_process;
}

