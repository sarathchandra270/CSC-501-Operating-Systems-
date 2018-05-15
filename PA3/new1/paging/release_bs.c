#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

	STATWORD 	ps;
	disable(ps);
	if(bs_id<0 || bs_id >=16)
	{
		restore(ps);
		return SYSERR;
	}
	
	int store = bs_id;
	
	//int pageth = -1;
	//bsm_lookup(currpid,vpno*4096,&store,&pageth);
	if(bsm_tab[store].bs_status == BSM_MAPPED)
	{
		
		if(bsm_tab[store].bs_priv == 1)
		{
			free_bsm(store);
			restore(ps);
			return OK;
		}
		else
		{
			int c=0;
			int i;
			int maxPages = bsm_tab[store].bs_maxPages;
			for(i=0;i<50;i++)
			{
				if(i!=currpid && bsm_tab[store].bs_pid[i]==1 && bsm_tab[store].bs_npages[i]>maxPages)
				{
					
					c++;
					maxPages = bsm_tab[store].bs_npages[i];
				}
				else if(i!=currpid && bsm_tab[store].bs_pid[i] == 1 && bsm_tab[store].bs_npages[i]<=maxPages)
				{
					
					c++;
				}
				else if (i == currpid && bsm_tab[store].bs_pid[i] != 1)
				{
					
					restore(ps);
					return SYSERR;
				}					
				else if(i == currpid && bsm_tab[store].bs_pid[i] == 1)
				{
					
					bsm_tab[store].bs_pid[i] = -1;
					bsm_tab[store].bs_npages[i] = 0;
					bsm_tab[store].bs_vpno[i] = 0;
				}
			}
			if(c == 0)
				{
					
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

