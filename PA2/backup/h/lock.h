
#define NLOCKS 50

#define LFREE   '\01'      
#define LUSED   '\02'

int lcreate(void);
int ldelete (int );
int lock (int , int , int );


struct lentry 
{
	char lstate;
	int lqhead;
	int lqtail; 

};

extern struct lentry lockarray[];

#define isbadlock(l)     (l<0 || l>=NLOCKS)
