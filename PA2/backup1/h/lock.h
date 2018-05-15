
#define NLOCKS 50

#define LFREE   '\01'      
#define LUSED   '\02'
#define LREAD   '\03'
#define LWRITE	'\04'
#define LDELETED '\05'

int lcreate(void);
int ldelete (int );
int lock (int , int , int );



struct lentry 
{
	char lstate;
	int lqhead;
	int lqtail; 
	int n_readers;

};

extern struct lentry lockarray[];
extern int nextlock;


#define isbadlock(l)     (l<0 || l>=NLOCKS)
