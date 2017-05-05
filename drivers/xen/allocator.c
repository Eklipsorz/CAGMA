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
#include <linux/Simple_rBuffer_Entry.h>

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

bool gotoWorkGen = 0;
bool enable_SupCenter = 0;

long long int CMA = -1;
long long int Mmax = 0 ;
long long int AVM = 0;

Simple_rBuffer_Entry rBuffer[rBuffer_Size];

EXPORT_SYMBOL_GPL(can_provide_mem);
EXPORT_SYMBOL_GPL(is_less_than_maxALM);
EXPORT_SYMBOL_GPL(enable_to_run_memAlloc);
EXPORT_SYMBOL_GPL(enable_SupCenter);
EXPORT_SYMBOL_GPL(gotoWorkGen);
EXPORT_SYMBOL_GPL(AVM);
EXPORT_SYMBOL_GPL(CMA);
EXPORT_SYMBOL_GPL(Mmax);
EXPORT_SYMBOL_GPL(rBuffer);

static void allocator_process(struct work_struct *work);
static DECLARE_DELAYED_WORK(allocator_worker, allocator_process);

static void allocator_process(struct work_struct *work)
{
	printk("Again\n");
	schedule_delayed_work(&allocator_worker, 3000);
	
}

void allocator_worker_gen(void)
{
	printk("Generating task\n");
	schedule_delayed_work(&allocator_worker, 10000);

}

EXPORT_SYMBOL_GPL(allocator_worker_gen);

static int __init allocator_init(void)
{

	pr_info("Initialising allocator driver\n");
	return 0;
}

subsys_initcall(allocator_init);

MODULE_LICENSE("GPL");
