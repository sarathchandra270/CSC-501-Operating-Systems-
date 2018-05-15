/* resume.c - resume */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * resume  --  unsuspend a process, making it ready; return the priority
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;

SYSCALL resume(int pid)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[9]=ctr1000;
        p->calls[9]=p->calls[9]+1;
        }
	STATWORD ps;    
	struct	pentry	*pptr;		/* pointer to proc. tab. entry	*/
	int	prio;			/* priority to return		*/

	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate!=PRSUSP) {
		restore(ps);
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[9]=p->calls_time[9]+ctr1000-p->initial_time[9];
        	}
		return(SYSERR);
	}
	prio = pptr->pprio;
	ready(pid, RESCHYES);
	restore(ps);
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[9]=p->calls_time[9]+ctr1000-p->initial_time[9];
        }
	return(prio);
}
