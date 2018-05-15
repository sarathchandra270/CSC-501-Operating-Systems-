#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) 
{
	STATWORD ps;
	disable(ps);
  /* requests a new mapping of npages with ID map_id */
 // kprintf("%d\n",npages);
	if(npages<=0 || npages>128 || bs_id<0 || bs_id >=16)
	{
		kprintf("helllo wrong\n");
		restore(ps);
		return(SYSERR);
	}
    if(bsm_tab[bs_id].bs_status==BSM_UNMAPPED)
	{
		kprintf("helllo right\n");
		bsm_tab[bs_id].bs_status = BSM_MAPPED;
		bsm_tab[bs_id].bs_pid[currpid] = 1;
		bsm_tab[bs_id].bs_priv = 0;
		//bsm_tab[bs_id].bs_vpno = 4096;
		bsm_tab[bs_id].bs_npages[currpid] = npages;
		bsm_tab[bs_id].bs_maxPages = npages;
		
		//bsm_tab[bs_id].bs_sem = 0;
	}
	else if(bsm_tab[bs_id].bs_status==BSM_MAPPED)
	{
		if(bsm_tab[bs_id].bs_priv == 1)
		{
			//if it is private, return SYSERR
			kprintf("helllo wrong private\n");
			restore(ps);
			return(SYSERR);
		}
		else
		{
			kprintf("PAGES are %d\n",bsm_tab[bs_id].bs_maxPages);
			//bsm_tab[bs_id].bs_status = BSM_MAPPED;
			unsigned int res = bsm_tab[bs_id].bs_npages;
				restore(ps);
				return bsm_tab[bs_id].bs_maxPages;
		}
	}
	restore(ps);
    return npages;
}


