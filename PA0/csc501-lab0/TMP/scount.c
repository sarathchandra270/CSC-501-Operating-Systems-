/* scount.c - scount */

#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include <proc.h>
/*------------------------------------------------------------------------
 *  scount  --  return a semaphore count
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;

SYSCALL scount(int sem)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[10]=ctr1000;
        p->calls[10]=p->calls[10]+1;
        }
extern	struct	sentry	semaph[];

	if (isbadsem(sem) || semaph[sem].sstate==SFREE)
		{
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[10]=p->calls_time[10]+ctr1000-p->initial_time[10];
        	}
		return(SYSERR);
		}
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[10]=p->calls_time[10]+ctr1000-p->initial_time[10];
        }
	return(semaph[sem].semcnt);
}
