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
//#define Mem_minimum 524288

/* 
 * This interface allows the system to be interrupted for doing the packet 
 * when it receives the packet.
 */

extern bool is_less_than_maxALM;
extern bool enable_to_run_memAlloc;
extern bool can_provide_mem ;
extern long long int Mmax,CMA;
extern int do_sysinfo(struct sysinfo *info);


MODULE_DESCRIPTION("Noify meminfo");
MODULE_AUTHOR("Orion <sslouis25@icloud.com>");
MODULE_LICENSE("GPL");

static void checkAVM(struct work_struct *);
static DECLARE_DELAYED_WORK(checkAVM_worker,checkAVM);


/*
 * 	Set template of a periodic task, called 'checkAVM_worker'
 */ 

static void checkAVM(struct work_struct *ws)
{
	struct xenbus_transaction trans;
	struct sysinfo sinfo;
	long long int availMem;

	do_sysinfo(&sinfo);
	availMem = sinfo.freeram >> 10;
	
	xenbus_transaction_start(&trans);
	xenbus_printf(trans, "memory","AVM", "%lld",availMem);
	xenbus_transaction_end(trans, 0);
	schedule_delayed_work(&checkAVM_worker,250);
}

/*
 *	Set template of a trigger to check whether ALM is bigger than maxALM 
 */ 

static void checkALM_watch(struct xenbus_watch *watch,
			      const char **vec, unsigned int len)
{
	struct xenbus_transaction trans;
	unsigned long long temp;
	char *Target;

	if (!is_less_than_maxALM)
	{
				
		xenbus_transaction_start(&trans);
		Target = (char *)xenbus_read(trans, "memory","target",NULL);
		xenbus_transaction_end(trans, 0);
		sscanf(Target,"%llu",&temp);

		if (temp < Mmax)
			is_less_than_maxALM = 1;	

	}

}

/*
 *	Set template of a event trigger to check whether hypervisor has sufficient memory.
 */ 

static void warning_watch(struct xenbus_watch *watch,
			      const char **vec, unsigned int len)
{
	struct xenbus_transaction trans;
	char *Target;

	
	xenbus_transaction_start(&trans);
	
	xenbus_printf(trans, "memory","CMA", "%lld",CMA);
	Target = (char *)xenbus_read(trans, "memory","warning",NULL);

	xenbus_transaction_end(trans, 0);
	
	if (Target[0] == '1')
		can_provide_mem = 1;
	else if(Target[0] == '0')
		can_provide_mem = 0;


}

/*
 *	define two event triggers, xbus_watch_target and xbus_watch_warning
 */ 

static struct xenbus_watch xbus_watch_target = {
	.node = "memory/target",
	.callback = checkALM_watch
};

static struct xenbus_watch xbus_watch_warning = {
	.node = "memory/warning",
	.callback = warning_watch
};


static int __init MemEventTrigger_init(void)
{

	int err;

	schedule_delayed_work(&checkAVM_worker,0);
	/*
 	 * 	register two evnet triggers into the system
 	 */  

	err = register_xenbus_watch(&xbus_watch_target);
	err = register_xenbus_watch(&xbus_watch_warning);
	
	can_provide_mem = 1;
	is_less_than_maxALM = 1;
	enable_to_run_memAlloc = 1;
	Mmax = 1536000; 
	
	return 0;

}

 
static void __exit MemEventTrigger_exit(void)
{
	is_less_than_maxALM = 0;
	can_provide_mem = 0;
	enable_to_run_memAlloc = 0;
	

	/*
 	 *	remove two event triggers from the system
 	 */ 
	unregister_xenbus_watch(&xbus_watch_target);
	unregister_xenbus_watch(&xbus_watch_warning);
	cancel_delayed_work(&checkAVM_worker);
	//cancel_delayed_work(&releaseMem_work);
	printk(KERN_INFO "Goodbye\n");
}
 
module_init(MemEventTrigger_init);
module_exit(MemEventTrigger_exit);
