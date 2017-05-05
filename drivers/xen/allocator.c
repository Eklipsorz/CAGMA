#include <linux/cpu.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/mm.h>
#include <linux/bootmem.h>
#include <linux/pagemap.h>
#include <linux/highmem.h>
#include <linux/mutex.h>
#include <linux/list.h>
#include <linux/gfp.h>
#include <linux/notifier.h>
#include <linux/memory.h>
#include <linux/memory_hotplug.h>
#include <linux/percpu-defs.h>
#include <linux/Simple_rBuffer.h>

#include <asm/page.h>
#include <asm/pgalloc.h>
#include <asm/pgtable.h>
#include <asm/tlb.h>

#include <asm/xen/hypervisor.h>
#include <asm/xen/hypercall.h>

#include <xen/xen.h>
#include <xen/interface/xen.h>
#include <xen/interface/memory.h>
#include <xen/balloon.h>
#include <xen/features.h>
#include <xen/page.h>
#include <xen/xenbus.h>         /* update CMA with xenbus     */

bool is_less_than_maxALM = 1;
bool can_provide_mem = 1;
bool enable_to_run_memAlloc = 0; 
bool enable_WorkGen = 1;

Simple_rBuffer_Entry rBuffer[rBuffer_Size];
long long int Mmax = 0 ;


EXPORT_SYMBOL_GPL(can_provide_mem);
EXPORT_SYMBOL_GPL(is_less_than_maxALM);
EXPORT_SYMBOL_GPL(enable_to_run_memAlloc);
EXPORT_SYMBOL_GPL(enable_WorkGen);
EXPORT_SYMBOL_GPL(Mmax);
EXPORT_SYMBOL_GPL(rBuffer);

static int curr_step = 0;

static void allocator_process(struct work_struct *work);
static DECLARE_DELAYED_WORK(allocator_worker, allocator_process);
extern int do_sysinfo(struct sysinfo *info);

static int Is_AVM_Bigger(void)
{
	struct sysinfo sinfo;
	long long int AVM,UMA;

	do_sysinfo(&sinfo);
	AVM = sinfo.freeram >> 10;
	UMA = (sinfo.totalram >> 10 ) - AVM;
	printk("UMA: %lld , AVM: %lld\n",UMA,AVM);
	if (AVM > UMA)
		return 1;
	else
		return 0;

}

static void allocate(long long int AVM, long long int CMA)
{
	struct xenbus_transaction trans;
	long long int out2lld,new_target,req;
	char *output;

	xenbus_transaction_start(&trans);
	output = (char *)xenbus_read(trans,"memory","target",NULL);
	xenbus_transaction_end(trans, 0);

	req = CMA - AVM;
	sscanf(output,"%lld",&out2lld);
	//req = (req * (long long int)((Alloc_rate)*1024)) >> 10;
	new_target = out2lld + req;
	
	if (new_target >= Mmax)
	{
		new_target = Mmax;	
		is_less_than_maxALM = 0;
	}
	
	xenbus_transaction_start(&trans);
	xenbus_printf(trans, "memory","target", "%lld", new_target); 
	xenbus_transaction_end(trans, 0);


}

static void reset_rBuffer(void)
{
	int i;

	for(i = 0; i < rBuffer_Size; i++)
	{
		rBuffer[i].CMA = 0;
		rBuffer[i].AVM = 0;	
	}

}


static void allocator_process(struct work_struct *work)
{
	int i;
	long long int CMA, AVM;

	if (Is_AVM_Bigger())
	{
		enable_WorkGen = 1;
		reset_rBuffer();
		return;
	}
	else
	{
		for(i = curr_step; i < rBuffer_Size; i++)
		{
			CMA = rBuffer[i].CMA;
			AVM = rBuffer[i].AVM;

			if(!CMA || !AVM)
			{
				curr_step = i;
				printk("none\n");
				break;
			}
			else
			{
				//allocate(CMA,AVM);			
				printk("CMA: %lld, AVM: %lld\n",CMA,AVM);	
				//rBuffer[i].CMA = 0;
				//rBuffer[i].AVM = 0;
			}
		}

		schedule_delayed_work(&allocator_worker,30);
	}
	
	
}

void allocator_worker_gen(void)
{
	printk("first generate\n");
	schedule_delayed_work(&allocator_worker, 0);
}

EXPORT_SYMBOL_GPL(allocator_worker_gen);

static int __init allocator_init(void)
{

	pr_info("Initialising allocator driver\n");
	return 0;
}

subsys_initcall(allocator_init);

MODULE_LICENSE("GPL");
