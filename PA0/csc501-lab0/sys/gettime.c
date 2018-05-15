/* gettime.c - gettime */

#include <conf.h>
#include <kernel.h>
#include <date.h>
#include <proc.h>
extern int getutim(unsigned long *);

/*------------------------------------------------------------------------
 *  gettime  -  get local time in seconds past Jan 1, 1970
 *------------------------------------------------------------------------
 */
unsigned long ctr1000;
extern int flag;

SYSCALL	gettime(long *timvar)
{
	if(flag==1)
        {
        struct pentry *p=&proctab[currpid];
        p->initial_time[4]=ctr1000;
        p->calls[4]=p->calls[4]+1;
        }
    /* long	now; */

	/* FIXME -- no getutim */
	if(flag==1)
        {
                struct pentry *p=&proctab[currpid];
                p->calls_time[4]=p->calls_time[4]+ctr1000-p->initial_time[4];
        }
    return OK;
}
