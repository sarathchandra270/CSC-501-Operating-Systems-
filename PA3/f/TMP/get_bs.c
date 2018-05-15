#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

int get_bs(bsd_t bs_id, unsigned int npages) 
{
	STATWORD ps;
	disable(ps);
  /* requests a new mapping of npages with ID map_id */
	if(npages<=0 || npages>128 || bs_id<0 || bs_id >=16)
	{
		restore(ps);
		return(SYSERR);
	}
    if(bsm_tab[bs_id].bs_status==BSM_UNMAPPED)
	{
		bsm_tab[bs_id].bs_status = BSM_MAPPED;
		bsm_tab[bs_id].bs_pid = currpid;
		//bsm_tab[bs_id].bs_vpno = 4096;
		bsm_tab[bs_id].bs_npages = npages;
		//bsm_tab[bs_id].bs_sem = 0;
	}
	else if(bsm_tab[bs_id].bs_status==BSM_MAPPED)
	{
		if(1)
		{
			//if it is private, return SYSERR
		}
		else
		{
			unsigned int res = bsm_tab[bs_id].bs_npages;
			restore(ps);
			return res;
		}
	}
	restore(ps);
    return npages;
}


