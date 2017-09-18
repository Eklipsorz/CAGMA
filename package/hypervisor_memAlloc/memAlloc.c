#include "memAlloc.h"

/* the length of the period for adjusting memory */
#define Period_Length_ms 3000					
#define Period_Length_us Period_Length_ms*1000

/* 
 * the variable represents whether there are sufficient memory
 * 	0: insufficient memory
 * 	1: sufficient memory
 */
static bool is_memory_sufficient = 0;

xentoollog_logger_stdiostream *logger = NULL;
libxl_ctx *ctx = NULL;
xenstat_node *cur_node = NULL;
xenstat_handle *xhandle = NULL;

/* import the four functions into this file from queue.c */
extern void allocate();
extern void release();
extern void listEntry();
extern void addEntry(int id,char *path,int64_t ALM,int64_t AVM,int64_t CMA);

/* check whether there are sufficient memory and return its result to each VM periodically */
static void check_And_noify_each_VM(libxl_dominfo *info,int NRvm)
{
	struct xs_handle *xs;
	xs_transaction_t trans;
	unsigned long AllfreeMem;
	char Warnpath[50],ret[1];
	int j;
	
	cur_node = xenstat_get_node(xhandle, XENSTAT_ALL);
	AllfreeMem = (xenstat_node_free_mem(cur_node)/1024);
	ret[0] = (AllfreeMem > 0 ? '1' : '0');

	if (ret[0]=='1')
		is_memory_sufficient = 1;
	else 
		is_memory_sufficient = 0;
	
	xs = xs_daemon_open();
		
	for (j = 1;j < NRvm;j++)
	{
		sprintf(Warnpath,"/local/domain/%d/memory/warning",info[j].domid);
		trans = xs_transaction_start(xs);
		xs_write(xs,trans,Warnpath,ret,1);
		xs_transaction_end(xs,trans,false);	
	
	}
	
	xs_daemon_close(xs);
}

/* obtain critical memory amount and available memory amount via xenstore and path in xenstore */
static void getInfoOfVM(char *path,int64_t *ALM,int64_t *AVM,int64_t *CMA)
{
	char *result[3], ALMPath[50], AVMPath[50], CMAPath[50];
	xs_transaction_t trans;
	struct xs_handle *xs;
	
		
	sprintf(ALMPath,"%s/target",path);	
	sprintf(AVMPath,"%s/AVM",path);	
	sprintf(CMAPath,"%s/CMA",path);
	xs = xs_daemon_open();
	trans = xs_transaction_start(xs);

	result[0] = (char *) xs_read(xs,trans,ALMPath,NULL);
	result[1] = (char *) xs_read(xs,trans,AVMPath,NULL);
	result[2] = (char *) xs_read(xs,trans,CMAPath,NULL);
	
	xs_transaction_end(xs,trans,false);	
	xs_daemon_close(xs);	
	
	sscanf(result[0],"%"PRId64,ALM);	
	sscanf(result[1],"%"PRId64,AVM);	
	sscanf(result[2],"%"PRId64,CMA);	

}

/* initialize interface that has sufficient permissions to read/write xenstore */
static void init_env()
{
	bool progress_use_cr;
	progress_use_cr=0;
	
	logger = xtl_createlogger_stdiostream(stderr, minmsglevel,\
		(progress_use_cr ? XTL_STDIOSTREAM_PROGRESS_USE_CR : 0));	

	if (libxl_ctx_alloc(&ctx, LIBXL_VERSION, 0, (xentoollog_logger*)logger)) {
		fprintf(stderr, "cannot init xl context\n");
		exit(1);
    	}
	
	xhandle = xenstat_init();

	if (xhandle == NULL)
		perror("Failed to initialize xenstat library\n");	
	
	cur_node = xenstat_get_node(xhandle, XENSTAT_ALL);

	
	
}


/* initialize two queues, called Alloc and Relea resprespectively */
static void init_LList()
{
	Alloc = (entry *) calloc(1,sizeof(entry));
	Alloc->next = Alloc;
	Alloc->prev = Alloc;
	
	Relea = (entry *) calloc(1,sizeof(entry));
	Relea->next = Relea;
	Relea->prev = Relea;
	
}	

/* the main function in memAlloc project */
int main()
{

	libxl_dominfo *info;
	int nb_domain,j,count = 0;
	char path[50];
	int64_t ALM,AVM,CMA;
	
	//printf("ans: %llu\n",xenstat_node_free_mem(cur_node)/1024);
	init_env();
	init_LList();
	/*
 	 * atexit(): Install interrupt handler  
 	 * it can run __cleanup__ when this process almost is closed.
 	 * However, if physical machine is shut down,  xl and xenstat will be automatically
 	 * cleaned up; therefore, we don't install interrupt handler.
 	 */
	 //if (atexit(__cleanup__) != 0)
         //    perror("Failed to install cleanup handler.\n");	
		

	while(1)
	{	
		info = libxl_list_domain(ctx, &nb_domain);
		check_And_noify_each_VM(info,nb_domain);
		for (j = 1;j < nb_domain;j++)
		{
			
			sprintf(path,"/local/domain/%d/memory",info[j].domid);
			
			getInfoOfVM(path,&ALM,&AVM,&CMA);	
			
			if (CMA == -1 || CMA == AVM)
				continue;	
			
			addEntry(info[j].domid, path, ALM, AVM, CMA);
		}
		
		//listEntry();
		if (is_memory_sufficient == '0')
			release();
		
		allocate();
		
		usleep(Period_Length_us);
		count++;
		if (count == 245)
			break;
	}
	return 0;
}
