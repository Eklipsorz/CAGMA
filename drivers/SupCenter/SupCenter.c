#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/Simple_rBuffer.h>

/* 
 * This interface allows the system to be interrupted for doing the packet 
 * when it receives the packet.
 */


static void _noify_(struct work_struct *);
static DECLARE_DELAYED_WORK(_noify_work,_noify_);

//extern int gotoWorkGen;
//extern int enable_SupCenter;
//extern Simple_rBuffer_Entry rBuffer[rBuffer_Size];

MODULE_DESCRIPTION("Noify meminfo");
MODULE_AUTHOR("Orion <sslouis25@icloud.com>");
MODULE_LICENSE("GPL");



static void _noify_(struct work_struct *ws)
{

/*	int j;
	for(j=0;j<rBuffer_Size;j++)
		printk("%d CMA: %lld AVM: %lld\n",j,rBuffer[j].CMA,rBuffer[j].AVM);

	printk("noify_again\n");
	schedule_delayed_work(&_noify_work,300);
*/
}

static void workGen(void)
{

	printk("generating work\n");
	schedule_delayed_work(&_noify_work,300);

}

EXPORT_SYMBOL_GPL(workGen);
static int __init NoifyMem_init(void)
{
	printk("hi\n");
/*	int i;
	for(i = 0; i < rBuffer_Size; i++)
	{
		rBuffer[i].AVM = 0;		
		rBuffer[i].CMA = 0;
	}
	return 0;
*/
	return 0;
}

 
static void __exit NoifyMem_exit(void)
{
	cancel_delayed_work(&_noify_work);
	printk(KERN_INFO "Goodbye\n");
}
 
module_init(NoifyMem_init);
module_exit(NoifyMem_exit);
