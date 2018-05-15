/* getpid.c - getpid */

#include <conf.h>
#include <kernel.h>
#include <proc.h>


/*------------------------------------------------------------------------
 * getpid  --  get the process id of currently executing process
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;

SYSCALL getpid()
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[2]=ctr1000;
        p->calls[2]=p->calls[2]+1;
        }
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[2]=p->calls_time[2]+ctr1000-p->initial_time[2];
        }
	return(currpid);
}
