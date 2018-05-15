#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>

SYSCALL release_bs(bsd_t bs_id) {

	// STATWORD 	ps;
	// disable(ps);
	// if(bs_id<0 || bs_id >=16)
	// {
		// restore(ps);
		// return SYSERR;
	// }
  // /* release the backing store with ID bs_id */
	// int k;
	// int count =0;
	// for(k=0;k<50;k++)
	// {
		// if(bsm_tab[bs_id].bs_pid[k]==-1)
		// {
			// count++;
		// }
		
	// }
	// if(count ==50)
	// {
		// free_bsm(bs_id);
		// restore(ps);
		// return OK;
	// }
	// //bsm_tab[bs_id].bs_status = BSM_UNMAPPED;
	// bsm_tab[bs_id].bs_pid[currpid] = -1;
	// bsm_tab[bs_id].bs_vpno[currpid] = 4096;
	// //bsm_tab[bs_id].bs_npages = 0;
	// //bsm_tab[bs_id].bs_priv = 0;
	// //bsm_tab[bs_id].bs_sem = 0;
	// restore(ps);
    // return OK;
                                       
}

