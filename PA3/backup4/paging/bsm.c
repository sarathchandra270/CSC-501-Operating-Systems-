/* bsm.c - manage the backing store mapping*/

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>

/*-------------------------------------------------------------------------
 * init_bsm- initialize bsm_tab
 *-------------------------------------------------------------------------
 */
 bs_map_t bsm_tab[16];
 
SYSCALL init_bsm()
{
	STATWORD 	ps;
	disable(ps);
	int i;
	for(i=0; i<16;i++)
	{
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		bsm_tab[i].bs_pid = -1;
		bsm_tab[i].bs_vpno = 4096;
		bsm_tab[i].bs_npages = 0;
		bsm_tab[i].bs_sem = 0;
		bsm_tab[i].bs_priv = 0;
	}
	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_bsm - get a free entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL get_bsm(int* avail)
{
	STATWORD 	ps;
	disable(ps);
	int i;
	for(i=0; i<16;i++)
	{
		if(bsm_tab[i].bs_status == BSM_UNMAPPED)
		{
			//kprintf("\nHello if");
			*avail = i;
			restore(ps);
			return(OK);
		}
	}
	restore(ps);
	return(SYSERR);
}


/*-------------------------------------------------------------------------
 * free_bsm - free an entry from bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL free_bsm(int i)
{
	STATWORD ps;
	disable(ps);
	bsm_tab[i].bs_status = BSM_UNMAPPED;
	bsm_tab[i].bs_pid = -1;
	bsm_tab[i].bs_vpno = 4096;
	bsm_tab[i].bs_npages = 0;
	bsm_tab[i].bs_sem = 0;
	bsm_tab[i].bs_priv = 0;
	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * bsm_lookup - lookup bsm_tab and find the corresponding entry
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_lookup(int pid, long vaddr, int* store, int* pageth)
{
	STATWORD 	ps;
	disable(ps);
	int i;
	for(i=0;i<16;i++)
	{
		if(bsm_tab[i].bs_pid == pid)
		{
			*store = i;
			*pageth = vaddr/4096 - bsm_tab[i].bs_vpno;
			restore(ps);
			return OK;
		}
	}
	restore(ps);
	return SYSERR;
}


/*-------------------------------------------------------------------------
 * bsm_map - add an mapping into bsm_tab 
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_map(int pid, int vpno, int source, int npages)
{
	STATWORD 	ps;
	disable(ps);
	if(source<0 || source >=16 || npages<=0 || npages>128)
	{
		restore(ps);
		return SYSERR;
	}
	bsm_tab[source].bs_status = BSM_MAPPED;
	bsm_tab[source].bs_pid = pid;
	bsm_tab[source].bs_vpno = vpno;
	bsm_tab[source].bs_npages = npages;
	
	proctab[pid].store = source;
	proctab[pid].vhpno = vpno;
	proctab[pid].bs_pmap[source] = 1;
	restore(ps);
	return OK;
}



/*-------------------------------------------------------------------------
 * bsm_unmap - delete an mapping from bsm_tab
 *-------------------------------------------------------------------------
 */
SYSCALL bsm_unmap(int pid, int vpno, int flag)
{
	STATWORD 	ps;
	disable(ps);
	int i;
	int store = -1;
	int pageth = -1;
	for(i=0; i<NFRAMES; i++)
	{
		if(frm_tab[i].fr_type == FR_PAGE && frm_tab[i].fr_pid == pid )
		{
			bsm_lookup(pid, (vpno*4096), &store, &pageth);
			write_bs((i+FRAME0)*NBPG, store, pageth);
		}
	}
	bsm_tab[store].bs_status = BSM_UNMAPPED;
	bsm_tab[store].bs_pid = -1;
	bsm_tab[store].bs_vpno = 4096;
	bsm_tab[store].bs_npages = 0;
	bsm_tab[i].bs_sem = 0;
	bsm_tab[i].bs_priv = 0;
	restore(ps);
	return OK;
}


