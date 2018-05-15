
#include<stdio.h>

static unsigned long *esp;
static unsigned long *ebp;
void printtos()
{
	int count=1;
	asm("mov %esp,esp");
	asm("mov %ebp,ebp");

	printf("\n \n");	
	printf("void printtos()\n");
	printf("Before[0x%08x]: 0x%08x \n", ebp+2, *(ebp+2));
	printf("After[0x%08x]: 0x%08x \n",ebp,*ebp);	
 	 
	while(count<5&&ebp>esp+count)	
	{
	printf("\t Element[0x%08x]: 0x%08x \n", esp+count, *(esp+count));	
	count++;
	}

}
