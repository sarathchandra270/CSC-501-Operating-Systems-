/* frame.c - manage physical frames */
#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <paging.h>
#include <proc.h>


/*-------------------------------------------------------------------------
 * init_frm - initialize frm_tab
 *-------------------------------------------------------------------------
 */
 fr_map_t frm_tab[NFRAMES];
extern int page_replace_policy;



// int itr;
// for(itr=0;itr<1024;itr++)
// {
	// replacearray[itr] = -1;
// }

 
SYSCALL init_frm()
{
	STATWORD 	ps;
	disable(ps);
	int i;
	for(i=0; i<NFRAMES;i++)
	{
		frm_tab[i].fr_status = FRM_UNMAPPED;
		frm_tab[i].fr_pid = -1;
		frm_tab[i].fr_vpno = 0;
		frm_tab[i].fr_refcnt = 0;
		frm_tab[i].fr_type = FR_PAGE;
		frm_tab[i].fr_dirty = 0;
		frm_tab[i].fr_counter = 0;
	}
	restore(ps);
	return OK;
}

/*-------------------------------------------------------------------------
 * get_frm - get a free frame according page replacement policy
 *-------------------------------------------------------------------------
 */ 
SYSCALL get_frm(int* avail)
{
	STATWORD 	ps;
	disable(ps);
	int i;
	for(i=0;i<NFRAMES;i++)
	{
		if(frm_tab[i].fr_status==FRM_UNMAPPED)
		{
			*avail = i;
			//kprintf("got frame %d\n",i);
			restore(ps);
			return(OK);
		}
	}
	if(page_replace_policy == SC)
	{
		int f_sc;
		f_sc = getFrameBySC();
		
		if(f_sc == -1)
		{
			restore(ps);
			return(SYSERR);
		}
		kprintf("Replacing Frame With SC Policy is: %d\n",f_sc);
		*avail = f_sc;
		restore(ps);
		return(OK);
	}
	if(page_replace_policy == LFU)
	{
		int f_lfu;
		f_lfu = getFrameByLFU();
		if(f_lfu == -1)
		{
			restore(ps);
			return(SYSERR);
		}
		kprintf("Replacing Frame With LFU Policy is: %d\n",f_lfu);
		*avail = f_lfu;
		restore(ps);
		return(OK);
	}
	restore(ps);
	return(SYSERR);
}

/*-------------------------------------------------------------------------
 * free_frm - free a frame 
 *-------------------------------------------------------------------------
 */
SYSCALL free_frm(int i)
{
	STATWORD ps;
	disable(ps);
	int store =-1;
	int pageth= -1;
	int pid = frm_tab[i].fr_pid;
	if(frm_tab[i].fr_type == FR_TBL || frm_tab[i].fr_type == FR_DIR)
	{
		restore(ps);
		return SYSERR;
	}
	
	//if the requested frame is page, then free it.
	
	unsigned long addr = frm_tab[i].fr_vpno * NBPG;
	virt_addr_t *ptr = (virt_addr_t *)&addr;
	if((bsm_lookup(currpid, addr, &store, &pageth)) != SYSERR || store != -1 || pageth !=-1)
	{
		write_bs((i+FRAME0)*NBPG, store, pageth);
	}
	frm_tab[i].fr_status = FRM_UNMAPPED;
	frm_tab[i].fr_pid = -1;
	frm_tab[i].fr_vpno = 0;
	frm_tab[i].fr_refcnt = 0;
	frm_tab[i].fr_type = FR_PAGE;
	/*for LFU policy */
	frm_tab[i].fr_counter = 0;
	replacedelete(i);
	pd_t *pd = ptr->pd_offset*4 + proctab[pid].pdbr;
	pt_t *pt = ptr->pt_offset*4 + pd->pd_base * NBPG;
	pt->pt_pres = 0;
	frm_tab[pd->pd_base - FRAME0].fr_refcnt--;
	if(frm_tab[pd->pd_base - FRAME0].fr_refcnt == 0)
	{
		pd->pd_pres = 0;
		frm_tab[pd->pd_base - FRAME0].fr_status = FRM_UNMAPPED;
		frm_tab[pd->pd_base - FRAME0].fr_pid = -1;
		frm_tab[pd->pd_base - FRAME0].fr_vpno = 4096;
		frm_tab[pd->pd_base - FRAME0].fr_refcnt = 0;
		frm_tab[pd->pd_base - FRAME0].fr_type = FR_PAGE;
	}
	restore(ps);
	return OK;
}

int getFrameBySC()
{	
	int i;
	int f_no = -1;
	int c=0;
	while(1)
	{
		//kprintf("rear is %d ref is %d\n",rear,ref);
		//kprintf("entered while\n");
		if(ref == rear+1)
		{
			//kprintf("entered base\n");
			ref = 0;
		}
		f_no = replacearray[ref];
		int replacevpno = frm_tab[f_no].fr_vpno ;
		int pid = frm_tab[f_no].fr_pid;
		unsigned long a = replacevpno *4096;
		virt_addr_t *ptr = (virt_addr_t *)&a;
		pd_t *pd = proctab[pid].pdbr + (ptr->pd_offset * 4);
		if(pd->pd_pres == 1)
		{
			//kprintf("entered while 1 %d\n",c++);
			pt_t *pt = pd->pd_base * NBPG + (ptr->pt_offset * 4);
			if(pt->pt_pres == 1)
			{
				//kprintf("entered while %d\n",a);
				if(pt->pt_acc == 1)
				{
					pt->pt_acc = 0;
					ref++;
				}
				else
				{
					//kprintf("entered break %d\n",f_no);
					//free_frm(ref);
					ref++;
					
					break;
				}
			}
		}
	}
	return f_no;
}

int getFrameByLFU()
{
	int i;
	int maxCount = -1;
	int maxVpno = -1;
	int index = -1;
	for(i=0;i<1024;i++)
	{
		if(frm_tab[i].fr_type == FR_PAGE)
		{
			if(frm_tab[i].fr_counter > maxCount)
			{
				maxCount = frm_tab[i].fr_counter;
				index = i;
			}
			else if(frm_tab[i].fr_counter == maxCount)
			{
				if(frm_tab[i].fr_vpno > frm_tab[index].fr_vpno)
				{
					maxCount = frm_tab[i].fr_counter;
					index = i;
				}
				else
				{
					maxCount = frm_tab[index].fr_counter;
				}
			}
			else
			{
				;
			}
		}
	}
	return index;
}


void replacedelete(int f)
{
	int i,k;
	for(i=0;i<=rear;i++)
	{
		if(replacearray[i]==f)
		{
			if(i<ref)
			{
				ref--;
			}
			break;
		}
		else
		{
			;
		}
	}
	for(k = i ; k<=rear ;k++)
	{
		replacearray[k] = replacearray[k+1];
	}
	rear--;
}
