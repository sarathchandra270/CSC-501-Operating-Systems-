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
  /* release the backing store with ID bs_id */
	bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
	bsm_tab[bs_id].bs_pid = -1;
	bsm_tab[bs_id].bs_vpno = 4096;
	bsm_tab[bs_id].bs_npages = 0;
	bsm_tab[bs_id].bs_priv = 0;
	bsm_tab[bs_id].bs_sem = 0;
	restore(ps);
    return OK;
                                       
}

