#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>

//#include <linux/vmstat.h>
#include <linux/mm.h>

#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/slab.h>

#include <xen/xenbus.h>         /* update CMA with xenbus     */
#include <xen/cagma.h>
//#define Mem_minimum 524288

/* 
 * This interface allows the system to be interrupted for doing the packet 
 * when it receives the packet.
 */
/*
extern bool is_less_than_maxALM;
extern bool enable_to_run_memAlloc;
extern bool can_provide_mem ;
extern long long int Mmax,CMA;
*/
extern int do_sysinfo(struct sysinfo *info);


MODULE_DESCRIPTION("Noify meminfo");
MODULE_AUTHOR("Orion <sslouis25@icloud.com>");
MODULE_LICENSE("GPL");

static void checkMFree(struct work_struct *);
static DECLARE_DELAYED_WORK(checkMFree_work,checkMFree);

/*static void releaseMem(struct work_struct *);
static DECLARE_DELAYED_WORK(releaseMem_work,releaseMem);

static void releaseMem(struct work_struct *ws)
{
	struct xenbus_transaction trans;
	struct sysinfo sinfo;
	long long int availMem,totalMem;
	long long int amount;
	char *Target;
	
	do_sysinfo(&sinfo);
	//AVM = sinfo.freeram >> 10;
	availMem = sinfo.freeram >> 10;
	
	if (availMem > CMA)
	{
	
		xenbus_transaction_start(&trans);
		Target = (char *)xenbus_read(trans, "memory","target",NULL);
		xenbus_transaction_end(trans, 0);
		sscanf(Target,"%lld",&totalMem);


		
		amount = availMem - CMA;
		if (totalMem - amount > Mem_minimum)
			totalMem = totalMem - amount;
		else		
			totalMem = Mem_minimum;
		printk("hitest hi\n");
		xenbus_transaction_start(&trans);
		xenbus_printf(trans, "memory","target","%lld",totalMem);
		xenbus_transaction_end(trans, 0);
		
	}
		schedule_delayed_work(&releaseMem_work,6000);


}
*/
static void checkMFree(struct work_struct *ws)
{
	struct xenbus_transaction trans;
	struct sysinfo sinfo;
	long long int availMem;

	do_sysinfo(&sinfo);
	//AVM = sinfo.freeram >> 10;
	availMem = sinfo.freeram >> 10;
	
	xenbus_transaction_start(&trans);
	xenbus_printf(trans, "memory","AVM", "%lld",availMem);
	xenbus_transaction_end(trans, 0);
	printk("check is_less_than_maxALM:%d\n",is_less_than_maxALM);
	schedule_delayed_work(&checkMFree_work,250);
}


static void checkALM_watch(struct xenbus_watch *watch,
			      const char **vec, unsigned int len)
{
	struct xenbus_transaction trans;
	unsigned long long temp;
	char *Target;
	
	xenbus_transaction_start(&trans);
	Target = (char *)xenbus_read(trans, "memory","target",NULL);
	xenbus_transaction_end(trans, 0);
	sscanf(Target,"%llu",&temp);
	
	if (!is_less_than_maxALM)
	{	
		if (temp < Mmax)
			is_less_than_maxALM = 1;	
	}
	else if (temp >= Mmax)
		is_less_than_maxALM = 0;

}

static void warning_watch(struct xenbus_watch *watch,
			      const char **vec, unsigned int len)
{
	struct xenbus_transaction trans;
	char *Target;

	//AVM = sinfo.freeram >> 10;
	
	xenbus_transaction_start(&trans);
	
	xenbus_printf(trans, "memory","CMA", "%lld",CMA);
	Target = (char *)xenbus_read(trans, "memory","warning",NULL);

	xenbus_transaction_end(trans, 0);
	
	if (Target[0] == '1')
		can_provide_mem = 1;
	else if(Target[0] == '0')
		can_provide_mem = 0;
//	printk(KERN_INFO "knock knock %d\n",can_provide_mem);	


}



static struct xenbus_watch xbus_watch_target = {
	.node = "memory/target",
	.callback = checkALM_watch
};

static struct xenbus_watch xbus_watch_warning = {
	.node = "memory/warning",
	.callback = warning_watch
};


static int __init supCenter_init(void)
{

	int err;

	schedule_delayed_work(&checkMFree_work,0);
	//schedule_delayed_work(&releaseMem_work,6000);
	err = register_xenbus_watch(&xbus_watch_target);
	err = register_xenbus_watch(&xbus_watch_warning);
//	xenbus_transaction_start(&trans);
//	output = (char *)xenbus_read(trans,"memory","static-max",NULL);	
//	xenbus_transaction_end(trans, 0);
//
//	sscanf(output,"%lld",&Mmax);
	//printk(KERN_INFO "HI I'M %lld\n",Mmax);
	can_provide_mem = 1;
	is_less_than_maxALM = 1;
	enable_to_run_memAlloc = 1;
	Mmax = 1536000; 
//	Mmax = 819200; 
	
	return 0;

}

 
static void __exit supCenter_exit(void)
{
	is_less_than_maxALM = 0;
	can_provide_mem = 0;
	enable_to_run_memAlloc = 0;
	unregister_xenbus_watch(&xbus_watch_target);
	unregister_xenbus_watch(&xbus_watch_warning);
	cancel_delayed_work(&checkMFree_work);
	//cancel_delayed_work(&releaseMem_work);
	printk(KERN_INFO "Goodbye\n");
}
 
module_init(supCenter_init);
module_exit(supCenter_exit);
