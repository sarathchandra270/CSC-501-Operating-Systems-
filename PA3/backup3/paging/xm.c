/* xm.c = xmmap xmunmap */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>


/*-------------------------------------------------------------------------
 * xmmap - xmmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmmap(int virtpage, bsd_t source, int npages)
{
	STATWORD 	ps;
	disable(ps);
	if(source<0 || source >15 || npages<=0 || npages>128 || virtpage < 4096)
	{
		restore(ps);
		return SYSERR;
	}
	bsm_map(currpid, virtpage, source, npages);
	restore(ps);
	return OK;
}



/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
	return SYSERR;
}
