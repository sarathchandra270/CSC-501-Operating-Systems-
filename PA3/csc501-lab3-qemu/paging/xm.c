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
	if(bsm_tab[source].bs_priv ==1 && bsm_tab[source].bs_status==BSM_MAPPED)
	{
		restore(ps);
		return OK;
	}
	if(bsm_tab[source].bs_status==BSM_UNMAPPED)
	{
		restore(ps);
		return SYSERR;
	}
	if(bsm_tab[source].bs_status==BSM_MAPPED && bsm_tab[source].bs_maxPages<npages)
	{
		restore(ps);
		return SYSERR;
	}
	else if(bsm_tab[source].bs_status==BSM_MAPPED)
	{
		bsm_tab[source].bs_pid[currpid] = 1;
		bsm_tab[source].bs_vpno[currpid] = virtpage;
		bsm_tab[source].bs_npages[currpid] = npages;
		restore(ps);
		return OK;
	}
	
}


/*-------------------------------------------------------------------------
 * xmunmap - xmunmap
 *-------------------------------------------------------------------------
 */
SYSCALL xmunmap(int virtpage)
{
	STATWORD 	ps;
	disable(ps);
	int flag =0;
	if(virtpage < 4096)
	{
		kprintf("Error in doing xmunmap : Wrong virtual page argument\n");
		restore(ps);
		return SYSERR;
	}
	else
	{
		bsm_unmap(currpid, virtpage, flag);
		restore(ps);
		return OK;
	}
	
}
