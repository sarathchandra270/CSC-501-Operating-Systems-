/* user.c - main */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>
/*------------------------------------------------------------------------
 *  main  --  user main program
 *------------------------------------------------------------------------
 */
int prX;
int prY;
int prZ;
void halt();

prch(c)
char c;
{
	int i;
	sleep(5);	
}

unsigned long *esp;

   								
int main()
{
	long result;
	kprintf("\n\nHello World, Xinu lives\n\n");
	result=zfunction(0xaabbccdd);
	//printf("%x", result);
	printsegaddress();

   
	printtos();
		
	
	syscallsummary_start();

	resume(prX = create(prch,2000,20,"proc X",1,'X'));
	resume(prY = create(prch,2000,20,"proc Y",1,'Y'));
	resume(prZ = create(prch,2000,20,"proc Z",1,'Z'));
	chprio(49,45);
	sleep(10);
		
	syscallsummary_stop();
	printprocstks(10);
	

	printsyscallsummary();
	

return 0;
}


