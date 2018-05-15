#include<stdio.h>

extern int etext,edata,end;

/*****************************************
Print the addresses indicating the end of the text, data, and BSS segments of the Xinu OS. 
Also print the 4-byte contents (in hexadecimal) preceding and after those addresses
******************************************/

void printsegaddress()
{
	printf("void printsegaddress()\n");
	printf("Current: ");
	printf("etext[0x%08x]=0x%08x, ", &etext, *(&etext));
	printf("edata[0x%08x]=0x%08x, ", &edata, *(&edata));
	printf("ebss[0x%08x]=0x%08x, ", &end, *(&end));
	printf("\n");

	printf("Preceding: ");
        printf("etext[0x%08x]=0x%08x, ", &etext-1, *(&etext-1));
        printf("edata[0x%08x]=0x%08x, ", &edata-1, *(&edata-1));
        printf("ebss[0x%08x]=0x%08x, ", &end-1, *(&end-1));
        printf("\n");

	printf("After: ");
        printf("etext[0x%08x]=0x%08x, ", &etext+1, *(&etext+1));
        printf("edata[0x%08x]=0x%08x, ", &edata+1, *(&edata+1));
        printf("ebss[0x%08x]=0x%08x, ", &end+1, *(&end+1));
        printf("\n");
}
