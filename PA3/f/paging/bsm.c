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
			bsm_tab[i].bs_vpno[j] = 0;
			bsm_tab[i].bs_npages[j] = 0;
		}

		bsm_tab[i].bs_sem = 0;
		bsm_tab[i].bs_priv = 0;
		bsm_tab[i].bs_maxPages = 0;
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
	bsm_tab[i].bs_vpno[j] = 0;
	bsm_tab[i].bs_npages[j] = 0;
	}
	
	bsm_tab[i].bs_sem = 0;
	bsm_tab[i].bs_priv = 0;
	bsm_tab[i].bs_maxPages = 0;
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
	bsm_tab[source].bs_npages[pid] = npages;
	
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
	int store = -1;
	int pageth = -1;
	bsm_lookup(pid,vpno*4096,&store,&pageth);
	kprintf("store is %d\n",bsm_tab[store].bs_status);
	if(bsm_tab[store].bs_status == BSM_MAPPED)
	{
		kprintf("store is %d\n",store);
		if(flag == 1)
		{
			free_bsm(store);
			restore(ps);
			return OK;
		}
		else
		{
			int c=0;
			int i;
			int maxPages =0;
			for(i=0;i<50;i++)
			{
				if(i!=pid && bsm_tab[store].bs_pid[i]==1 && bsm_tab[store].bs_npages[i]>maxPages)
				{
					kprintf("inside serriir\n");
					c++;
					maxPages = bsm_tab[store].bs_npages[i];
				}
				else if(i!=pid && bsm_tab[store].bs_pid[i] == 1 && bsm_tab[store].bs_npages[i]<=maxPages)
				{
					kprintf("inside serriir1111111111\n");
					c++;
				}
				else if (i == pid && bsm_tab[store].bs_pid[i] != 1)
				{
					restore(ps);
					return SYSERR;
				}					
				else if(i == pid && bsm_tab[store].bs_pid[i] == 1)
				{
					bsm_tab[store].bs_pid[i] = -1;
					bsm_tab[store].bs_npages[i] = 0;
					bsm_tab[store].bs_vpno[i] = 0;
				}
			}
			if(c == 0)
				{
					kprintf("i am coming here\n");
					free_bsm(store);
				}
				else
				{
					bsm_tab[store].bs_maxPages = maxPages;
				}
		}
	}
	
	restore(ps);
	return OK;
}


