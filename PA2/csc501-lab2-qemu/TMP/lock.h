
#define NLOCKS 50

#define LFREE   '\01'      
#define LUSED   '\02'
#define LREAD   '\03'
#define LWRITE	'\04'
//#define LDELETED '\05'

#define READ 10
#define WRITE 11

int lcreate();
int ldelete (int );
int lock (int , int , int );
int releaseall(int numlocks, int ldes1, ...);
void linit();

struct lentry 
{
	char lstate;
	int lqhead;
	int lqtail; 
	int n_readers;
	int maxprio;
	int del_wait[50];
	int pwait[50];
	int plist[50];   /*list of process currently holding a lock*/
};

extern struct lentry lockarray[NLOCKS];
extern int nextlock;


#define isbadlock(l)     (l<0 || l>=NLOCKS)
