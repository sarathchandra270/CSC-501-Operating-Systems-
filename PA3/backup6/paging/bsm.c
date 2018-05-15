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
	int i,j;
	for(i=0; i<16;i++)
	{
		bsm_tab[i].bs_status = BSM_UNMAPPED;
		for(j=0;j<50;j++)
		{
			bsm_tab[i].bs_pid[j] = -1;
			bsm_tab[i].bs_vpno[j] = 4096;
			
		}
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
	int j;
	bsm_tab[i].bs_status = BSM_UNMAPPED;
	for(j=0;j<50;j++)
	{
	bsm_tab[i].bs_pid[j] = -1;
	bsm_tab[i].bs_vpno[j] = 4096;
	}
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
	//kprintf("this is lookup\n");
	for(i=0;i<16;i++)
	{
		if(bsm_tab[i].bs_pid[pid] == 1)
		{
			*store = i;
			*pageth = vaddr/4096 - bsm_tab[i].bs_vpno[pid];
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
	bsm_tab[source].bs_pid[pid] = 1;
	bsm_tab[source].bs_vpno[pid] = vpno;
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
			free_frm(i);
		}
	}
		unsigned long addr = vpno * NBPG;
		virt_addr_t *ptr = (virt_addr_t*)&addr;
		pd_t *pd = proctab[pid].pdbr + (ptr->pd_offset * 4);
		pt_t *pt = pd->pd_base * NBPG + (ptr->pt_offset * 4);
		pt->pt_pres = 0;
	int k;
	int count =0;
	for(k=0;k<50;k++)
	{
		if(bsm_tab[store].bs_pid[k]==-1)
		{
			count++;
		}
		
	}
	if(count ==50)
	{
		free_bsm(store);
		restore(ps);
		return OK;
	}
	//bsm_tab[store].bs_status = BSM_UNMAPPED;
	bsm_tab[store].bs_pid[pid] = -1;
	bsm_tab[store].bs_vpno[pid] = 4096;
	//bsm_tab[store].bs_npages = 0;
	//bsm_tab[i].bs_sem = 0;
	//bsm_tab[i].bs_priv = 0;
	restore(ps);
	return OK;
}


