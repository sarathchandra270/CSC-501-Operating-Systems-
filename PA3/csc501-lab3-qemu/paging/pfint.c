/* pfint.c - pfint */

#include <conf.h>
#include <kernel.h>
#include <paging.h>
#include <proc.h>


/*-------------------------------------------------------------------------
 * pfint - paging fault ISR
 *-------------------------------------------------------------------------
 */
 
int replacearray[1024];
int front = -1;
int rear = -1;
int ref = -1;

SYSCALL pfint()
{
	STATWORD ps;
	disable(ps);
	unsigned long a = read_cr2();
	
	int store = -1;
	int pageth = -1;
	virt_addr_t *ptr = (virt_addr_t *)&a;
	if((bsm_lookup(currpid, a, &store, &pageth)) == SYSERR)
	{
		kprintf("Error in pfint: Address is not a valid address\n");
		kill(currpid);
		restore(ps);
		return SYSERR;
	}
	
	if(store == -1 || pageth ==-1)
	{
		kprintf("Error in pfint: Address is not a valid address\n");
		kill(currpid);
		restore(ps);
		return SYSERR;
	}
	
	pd_t *pd = proctab[currpid].pdbr + (ptr->pd_offset * 4);
	
	if(pd->pd_pres == 0)
	{
		int f_no;
		f_no = createPageTable(currpid);
		if(f_no==-1)
		{
			kprintf("Error in creating page table in pfint\n");
			kill(currpid);
			restore(ps);
			return SYSERR;
		}
		pd->pd_pres = 1;
		pd->pd_write = 1;
		pd->pd_pcd = 0;
		pd->pd_acc = 0;
		pd->pd_global = 0;
		pd->pd_pwt = 0;
		pd->pd_mbz = 0;
		pd->pd_user = 1;
		pd->pd_fmb = 0;
		pd->pd_avail = 0;
		pd->pd_base = FRAME0 + f_no;
	
	pt_t *pt = pd->pd_base * NBPG + (ptr->pt_offset * 4);
		
		int f_num = -1;
		get_frm(&f_num);
		if(f_num==-1)
		{
			kprintf("Error in creating page in pfint\n");
			kill(currpid);
			restore(ps);
			return SYSERR;
		}
		
		//initialize values for frame
		frm_tab[f_num].fr_status = FRM_MAPPED;
		frm_tab[f_num].fr_pid = currpid;
		frm_tab[f_num].fr_type = FR_PAGE;
		frm_tab[f_num].fr_vpno = a/NBPG;
		frm_tab[f_num].fr_counter += 1;
		read_bs(((f_num + FRAME0)*NBPG),store,pageth);
		
		//initialize page table entry
		pt->pt_pres =1;
		pt->pt_acc = 1;
		pt->pt_write = 1;
		pt->pt_base = f_num + FRAME0;
		replaceinsert(f_num);
	}
	else
	{
		pt_t *pt = pd->pd_base * NBPG + (ptr->pt_offset * 4);
		int f_num = -1;
		get_frm(&f_num);
		if(f_num==-1)
		{
			kprintf("Error in creating page in pfint\n");
			kill(currpid);
			restore(ps);
			return SYSERR;
		}
		frm_tab[f_num].fr_status = FRM_MAPPED;
		frm_tab[f_num].fr_pid = currpid;
		frm_tab[f_num].fr_type = FR_PAGE;
		frm_tab[f_num].fr_vpno = a/NBPG;
		frm_tab[f_num].fr_counter += 1;
		frm_tab[pd->pd_base - FRAME0].fr_refcnt++;
		read_bs(((f_num + FRAME0)*NBPG),store,pageth);
		//initialize page table entry
		pt->pt_pres =1;
		pt->pt_acc = 1;
		replaceinsert(f_num);
		pt->pt_write = 1;
		pt->pt_base = f_num + FRAME0;
	}
	restore(ps);
	return OK;
}

int createPageTable(int pid)
{
	int f =-1;
	pt_t *page_ptr;
	int j;
	get_frm(&f);
	if(f == -1)
	{
		return SYSERR;
	}
		frm_tab[f].fr_status = FRM_MAPPED;
		frm_tab[f].fr_pid = pid;
		frm_tab[f].fr_type = FR_TBL;
		frm_tab[f].fr_refcnt++;
		
		page_ptr = (pt_t *)((FRAME0+f) * NBPG);
		
		for(j=0;j<1024;j++)
		{
			page_ptr->pt_pres = 0;
			page_ptr->pt_write = 1;
			page_ptr->pt_user = 0;
			page_ptr->pt_pwt = 0;
			page_ptr->pt_pcd = 0;
			page_ptr->pt_acc = 0;
			page_ptr->pt_dirty = 0;
			page_ptr->pt_mbz = 0;
			page_ptr->pt_global = 0;
			page_ptr->pt_avail = 0;
			//putting frame number since address is 32 bit but pt_base is 20bit
			page_ptr->pt_base = 0; 
			page_ptr++;
		}
	return f;
}

void replaceinsert(int frame)
{
	if( rear == -1 && ref == -1)
	{
		rear = ref = 0;
	}
	else
	{
		rear++;
	}
	replacearray[rear] = frame;
}
