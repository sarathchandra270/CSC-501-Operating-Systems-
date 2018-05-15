/* setdev.c - setdev */

#include <conf.h>
#include <kernel.h>
#include <proc.h>

/*------------------------------------------------------------------------
 *  setdev  -  set the two device entries in the process table entry
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;
SYSCALL	setdev(int pid, int dev1, int dev2)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[13]=ctr1000;
        p->calls[13]=p->calls[13]+1;
        }
	short	*nxtdev;

	if (isbadpid(pid))
		{
		if(flag==1)
        	{
                struct pentry *p=&proctab[currpid];
                p->calls_time[13]=p->calls_time[13]+ctr1000-p->initial_time[13];
        	}
		return(SYSERR);
		}
	nxtdev = (short *) proctab[pid].pdevs;
	*nxtdev++ = dev1;
	*nxtdev = dev2;
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[13]=p->calls_time[13]+ctr1000-p->initial_time[13];
        }
	return(OK);
}
