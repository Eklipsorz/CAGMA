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

extern int do_sysinfo(struct sysinfo *info);


MODULE_DESCRIPTION("Noify meminfo");
MODULE_AUTHOR("Orion <sslouis25@icloud.com>");
MODULE_LICENSE("GPL");

static void checkMFree(struct work_struct *);
static DECLARE_DELAYED_WORK(checkMFree_work,checkMFree);

/* 
 * The template of a task, which periodically update the value
 * of availabe memory in this machine.
 */
static void checkMFree(struct work_struct *ws)
{
	struct xenbus_transaction trans;
	struct sysinfo sinfo;
	long long int availMem;

	/* obtain unused memory amount via do_sysinfo() */
	do_sysinfo(&sinfo);
	availMem = sinfo.freeram >> 10;

	/* update the new amount into memory/AVM in xenstore */
	xenbus_transaction_start(&trans);
	xenbus_printf(trans, "memory","AVM", "%lld",availMem);
	xenbus_transaction_end(trans, 0);

	/* schedule a delayable task for each 250ms */
	schedule_delayed_work(&checkMFree_work,250);
}

/* set a callback function of memory/target watch. */
static void checkALM_watch(struct xenbus_watch *watch,
			      const char **vec, unsigned int len)
{
	struct xenbus_transaction trans;
	unsigned long long temp;
	char *Target;
	
	/* obtain allocated memory amount via xenstore */
	xenbus_transaction_start(&trans);
	Target = (char *)xenbus_read(trans, "memory","target",NULL);
	xenbus_transaction_end(trans, 0);

	/* transfer string into unsigned long long int */
	sscanf(Target,"%llu",&temp);

	/* 
	 * check whether the allocated memory amount is bigger than
	 * maximum amount. If so, the guest OS set is_less_than_maxALM
	 * to 0; if not, is_less_than_maxALM is set to 1.
	 */
	if (!is_less_than_maxALM)
	{	
		if (temp < Mmax)
			is_less_than_maxALM = 1;	
	}
	else if (temp >= Mmax)
		is_less_than_maxALM = 0;

}

/* set a callback function of memory/warning watch */
static void warning_watch(struct xenbus_watch *watch,
			      const char **vec, unsigned int len)
{
	struct xenbus_transaction trans;
	char *Target;


	/* obtain the value of memory/warning in xenstore and update the value of memory/CMA. */
	xenbus_transaction_start(&trans);
	
	xenbus_printf(trans, "memory","CMA", "%lld",CMA);
	Target = (char *)xenbus_read(trans, "memory","warning",NULL);

	xenbus_transaction_end(trans, 0);

	/* set can_provide_mem to 0/1 according to the value of memory/warning */
	/* 
	 * If the value is 1, it represents that there are sufficient physical memory
	 * If not, it represents that there is no any physical memory
	 */
	if (Target[0] == '1')
		can_provide_mem = 1;
	else if(Target[0] == '0')
		can_provide_mem = 0;


}


/* set a watch to monitor whether memory/target is changed */
static struct xenbus_watch xbus_watch_target = {
	.node = "memory/target",
	.callback = checkALM_watch
};

/* set a watch to monitor whether memory/warning is changed */
static struct xenbus_watch xbus_watch_warning = {
	.node = "memory/warning",
	.callback = warning_watch
};

/* initialize supCenter module */
static int __init supCenter_init(void)
{

	int err;

	/* schedule a delayable task after 0ms */
	schedule_delayed_work(&checkMFree_work,0);

	/* register two watches, called memory/target and memory/warning */
	err = register_xenbus_watch(&xbus_watch_target);
	err = register_xenbus_watch(&xbus_watch_warning);
	
	/* initialize the four variables */
	can_provide_mem = 1;
	is_less_than_maxALM = 1;
	enable_to_run_memAlloc = 1;
	Mmax = 1536000; 
	
	return 0;

}

/* this function to be called at module removeal time */ 
static void __exit supCenter_exit(void)
{
	/* reset the three variables */
	is_less_than_maxALM = 0;
	can_provide_mem = 0;
	enable_to_run_memAlloc = 0;

	/* remove the two watches from guest OS */
	unregister_xenbus_watch(&xbus_watch_target);
	unregister_xenbus_watch(&xbus_watch_warning);

	/* remove all dalayable generated from template checkMFree_work	in scheduler */
	cancel_delayed_work(&checkMFree_work);
	
	printk(KERN_INFO "Goodbye\n");
}
 
module_init(supCenter_init);
module_exit(supCenter_exit);
