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

bool is_less_than_maxALM = 1;
bool can_provide_mem = 1;
bool enable_to_run_memAlloc = 0; 
bool enable_WorkGen = 0;

Simple_rBuffer_Entry rBuffer[rBuffer_Size];
long long int Mmax = 0 ;


EXPORT_SYMBOL_GPL(can_provide_mem);
EXPORT_SYMBOL_GPL(is_less_than_maxALM);
EXPORT_SYMBOL_GPL(enable_to_run_memAlloc);
EXPORT_SYMBOL_GPL(enable_WorkGen);
EXPORT_SYMBOL_GPL(Mmax);
EXPORT_SYMBOL_GPL(rBuffer);

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
	if (AVM > UMA)
		return 1;
	else
		return 0;

}

static void allocate(long long int AVM, long long int CMA)
{


}

static void allocator_process(struct work_struct *work)
{
	int i;
	long long int CMA, AVM;

	if (Is_AVM_Bigger())
	{
		enable_WorkGen = 0;
		return;
	}
	else
	{
		for(i = 0; i < rBuffer_Size; i++)
		{
			CMA = rBuffer[i].CMA;
			AVM = rBuffer[i].AVM;

			if(!CMA || !AVM)
				break;
			else
			{
				allocate(CMA,AVM);			
				rBuffer[i].CMA = 0;
				rBuffer[i].AVM = 0;
			}
		}

		schedule_delayed_work(&allocator_worker,30);
	}
	
	
}

void allocator_worker_gen(void)
{
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
