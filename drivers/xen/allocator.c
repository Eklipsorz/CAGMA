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
#define Alloc_rate 1.1

bool is_less_than_maxALM = 1;
bool can_provide_mem = 1;
bool enable_to_run_memAlloc = 0; 

long long int Mmax;
long long int CMA;
long long int count = 0;


EXPORT_SYMBOL_GPL(can_provide_mem);
EXPORT_SYMBOL_GPL(is_less_than_maxALM);
EXPORT_SYMBOL_GPL(enable_to_run_memAlloc);
EXPORT_SYMBOL_GPL(Mmax);
EXPORT_SYMBOL_GPL(CMA);

static void allocator_process(struct work_struct *work);
static DECLARE_DELAYED_WORK(allocator_worker, allocator_process);
extern int do_sysinfo(struct sysinfo *info);
extern struct task_struct *find_lock_task_mm(struct task_struct *p);

static void allocate(long long int AVMtemp, long long int CMAtemp)
{
	struct xenbus_transaction trans;
	long long int out2lld,new_target,req;
	char *output;

	xenbus_transaction_start(&trans);
	output = (char *)xenbus_read(trans,"memory","target",NULL);
	xenbus_transaction_end(trans, 0);

	req = CMAtemp - AVMtemp;
	sscanf(output,"%lld",&out2lld);
	//req = (req * (long long int)((Alloc_rate)*1024)) >> 10;
	new_target = out2lld + req;
	printk("new_target:%lld \n",new_target);	
	if (new_target >= Mmax)
	{
		new_target = Mmax;	
		is_less_than_maxALM = 0;
	}
	
	xenbus_transaction_start(&trans);
	xenbus_printf(trans, "memory","target", "%lld", new_target); 
	xenbus_transaction_end(trans, 0);


}



static void allocator_process(struct work_struct *work)
{
//	long long int CMAtemp, AVMtemp;	
	struct task_struct *task;
	unsigned long CMA = 0,temp; 
//	struct sysinfo sinfo;
	
//	do_sysinfo(&sinfo);
//	AVM = sinfo.freeram >> 10;
	for_each_process(task)
	{
		struct task_struct *p;
		
		p = find_lock_task_mm(task);
		if (!p)
			continue;
		cpumask_clear_cpu(1,mm_cpumask(p->mm));
		task_unlock(p);
		
		temp = get_mm_rss(p->mm) + get_mm_counter(p->mm, MM_SWAPENTS);
		if (temp > CMA)
			CMA = temp;
		printk("temp = %lu, CMA = %lu\n",temp,CMA);
		//printk("[%d] = %lu\n",p->pid,get_mm_rss(p->mm));
	}
	printk("final CMA = %lu \n",CMA);

//	tsk = current;
//	CMAtemp = ((long long int) tsk->mm->hiwater_rss) << 2;
//	CMAtemp = (CMAtemp * (long long int)(Alloc_rate*1024)) >> 10;
	
	//printk("CMAtemp: %lld, AVM: %lld", CMAtemp,AVM);
//	printk("CMAtemp: %lld\n", CMAtemp);
	
/*	if (AVM < CMAtemp && CMA != CMAtemp)	
	{
		CMA = CMAtemp;		

	}
*/
/*
	if (Is_AVM_Bigger())
	{
		count++;
		enable_WorkGen = 1;
		printk("count:%lld \n",count);
		return;
	}
	else
	{
		CMAtemp = rBuffer.CMA;
		AVMtemp = rBuffer.AVM;
		
		if ( CMAtemp > 0 && AVMtemp > 0)
			allocate(AVMtemp,CMAtemp);			
		//printk("CMA: %lld, AVM: %lld\n", CMAtemp, AVMtemp);	
		schedule_delayed_work(&allocator_worker,300);
	}
*/	
	
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
