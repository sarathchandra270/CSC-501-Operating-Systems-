#include <conf.h>
//#include <kernel.h>

//#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <sem.h>
#include <stdio.h>
#include <lock.h>

LOCAL int newlock();

/*------------------------------------------------------------------------
 * screate  --  create and initialize a semaphore, returning its id
 *------------------------------------------------------------------------
 */
int lcreate()
{
	STATWORD ps;    
	int lock;

	disable(ps);
	if ((lock=newlock())==SYSERR ) {
		restore(ps);
		return(SYSERR);
	}
	restore(ps);
	return(lock);
	
}

/*------------------------------------------------------------------------
 * newsem  --  allocate an unused semaphore and return its index
 *------------------------------------------------------------------------
 */
LOCAL int newlock()
{
	int	lock;
	int	i;

	for (i=0 ; i<NLOCKS ; i++) {
		lock=nextlock--;
		if (nextlock < 0)
			nextlock = NLOCKS-1;
		if (lockarray[lock].lstate==LFREE) {
			lockarray[lock].lstate = LUSED;
			return(lock);
		}
	}
	return(SYSERR);
} 