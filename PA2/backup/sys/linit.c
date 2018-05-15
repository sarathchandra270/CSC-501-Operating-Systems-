#include <lock.h>
#include <q.h>
#include <kernel.h>
#include <proc.h>

void linit()
{
	int i;
	struct lentry *lptr;
	for (i=0 ; i<NLOCKS ; i++) 
	{
	      /* initialize locks */
                (lptr = &lockarray[i])->lstate = LFREE;
                lptr->lqtail = 1 + (lptr->lqhead = newqueue());
        }

}
