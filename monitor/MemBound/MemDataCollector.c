#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>

#include <linux/mm.h>

#include <linux/fs.h>
#include <linux/debugfs.h>
#include <linux/slab.h>

#include <net/sock.h>
#include <net/netlink.h>
#include <linux/signal.h>
#include <linux/pid.h>
#include <linux/proc_fs.h>

#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/semaphore.h>
#define collect_period 3000

#define PROCFS_MAXSIZE 512
#define PROCFS_FILESUFFEX_SETTER "fsuffexSetter"
#define PROCFS_BUFFER "buffer"
#define MainProg "FixedAccMem"

/* declare the template of task */
static void round_counter_(struct work_struct *);
static DECLARE_DELAYED_WORK(round_counter_work,round_counter_);

MODULE_DESCRIPTION("Noify meminfo");
MODULE_AUTHOR("Orion <sslouis25@icloud.com>");
MODULE_LICENSE("GPL");

static char procbuffer[PROCFS_MAXSIZE];

static bool enable_to_begin = 0;
static int fileNum;
static int round = 0;



MODULE_DESCRIPTION("Noify meminfo");
MODULE_AUTHOR("Orion <sslouis25@icloud.com>");
MODULE_LICENSE("GPL");


/* set a callback function on the time a file is opened */
static struct file* file_open(const char* path, int flags, int rights) 
{	
	struct file* filp = NULL;
	mm_segment_t oldfs;
	int err = 0;

	oldfs = get_fs();
	set_fs(get_ds());
	   
	filp = filp_open(path, flags, rights);
	set_fs(oldfs);
		         
	if(IS_ERR(filp)) 
	{
		err = PTR_ERR(filp);
		return NULL;
	}

	return filp;
}

/* set a callback function on the time a file is closed */
void file_close(struct file* file) {
	filp_close(file, NULL);
}


/* set a callback function on the time a file is written */	
int file_write(struct file* file, unsigned long long offset, unsigned char* data, unsigned int size) 
{
	mm_segment_t oldfs;
	int ret;
	
	oldfs = get_fs();
	set_fs(get_ds());
	ret = vfs_write(file, data, size, &offset);
        
	set_fs(oldfs);
		    
	return ret;

}

/* set a timer to count the times for collecting data */ 
static void round_counter_(struct work_struct *ws)
{
	/*
	 * when the DataCollector is enabled, it begins to count
	 * the time for each collect_period in millisecond.  
	 */
	if(enable_to_begin)
		round++;

	schedule_delayed_work(&round_counter_work,collect_period);
	
}



/*********************************************************
 *                  ProcFS section Beginning 
 *********************************************************/

/* 
 * set a callback of /proc/buffer to handle collecting the 
 * data from each memory-bound task 
 */
static ssize_t buffer_write(struct file *file, const char *buffer, size_t count,loff_t *data)
{
 	struct semaphore sem; 
	struct file *_file_ = NULL;
	char filepath[30],context[20];

	/* initialize semaphore and its number */
	sema_init(&sem,1);

	/* set the variable enable_to_begin to limit the activation of DataCollector */
	/* When the /proc/buffer is accessed, the DataCollector is activated 	     */ 
	if(!enable_to_begin)
	{
		/* initialize the number of round */
		round = 1;
		enable_to_begin = 1;
	} 
	else 
	{
		if(count > PROCFS_MAXSIZE)
       			count = PROCFS_MAXSIZE; 
   		if (copy_from_user(procbuffer,buffer,count))    
       			return -EFAULT;
	
		/* obtain the path of the file, which stores all data from each task */	
		sprintf(filepath,"/root/result/MEM_RTIME.%d",fileNum);
		sprintf(context,"%d\t%s\n",round,procbuffer);

	
		/* collected data is written into the file _file_ */  	
		_file_ = file_open(filepath, O_RDWR | O_APPEND | O_CREAT, 0644);
		/* 
		 * down() and up() can avoid the resource contention on the time
		 * a large number of call of file_write() are generated to schedule
		 */ 
		down(&sem);
			file_write(_file_,_file_->f_pos,context,strlen(context)+1);		
		up(&sem);
			
		file_close(_file_);
	}
		
   	return count;
}

/* 
 * set a callback function of /proc/enabler. when reading or writing it,
 * the system call this function to activate Data Collector 
 */
static ssize_t _handling_notification_(struct file *file, const char *buffer, size_t count,loff_t *data)
{
	char fileNumTemp[PROCFS_MAXSIZE];

	/* set the maximum number of the words in the fileNumTemp */
	if(count > PROCFS_MAXSIZE)
       		count = PROCFS_MAXSIZE; 
	/* receive a string stored in /proc/enabler */
	if (copy_from_user(fileNumTemp,buffer,count))    
       		return -EFAULT;

	/* transfer string into int */
	sscanf(fileNumTemp,"%d",&fileNum);

	/* schedule a delayable task */
	schedule_delayed_work(&round_counter_work,0);

	return 0;
}

/* 
 * Create /proc/fsuffexSetter in /proc to define the suffex of the 
 * file, which stores all data generated from each memory-bound task
 */
static int create_procfs_fsuffex_setter(void)
{
	static struct proc_dir_entry *p;
	
	/* set a callback function on the time the file is written */
	static const struct file_operations proc_file_fops = {
		.owner = THIS_MODULE,
		.write = _handling_notification_,
	};
	
	/* create a node in /proc and set permission */
	p = proc_create(PROCFS_FILESUFFEX_SETTER, S_IRWXU, NULL, &proc_file_fops);

	if (!p) {
		printk("%s(#%d): create proc entry failed\n", __func__, __LINE__);        
		return -EFAULT;
	}

	return 0;

}

/* 
 * Create entry /proc/buffer in /proc to collect the data 
 * from each memory-bound task 
 */
static int create_procfs_buffer(void)
{
	static struct proc_dir_entry *p;
	
	/* set a callback function on the time the file is written */
	static const struct file_operations proc_file_fops = {
		.owner = THIS_MODULE,
		.write = buffer_write,
	};    
	
	/* create a node in /proc and set permission */
   	p = proc_create(PROCFS_BUFFER, S_IRWXU, NULL, &proc_file_fops);
        
	if (!p) {
		printk("%s(#%d): create proc entry failed\n", __func__, __LINE__);		
		return -EFAULT;
	}
	return 0;
}


/*********************************************************
 *                  ProcFS section End
 *********************************************************/


/* initialize this module */
static int __init MemDataCollector_init(void)
{
	
	/* create two entries in /proc, called buffer and enabler */
	create_procfs_buffer();
	create_procfs_fsuffex_setter();
	
	return 0;
}

 
/* this function to be called at module removeal time */ 
static void __exit MemDataCollector_exit(void)
{

	/* 
	 * remove all dalayable generated from template 
	 * _notify_to_process_work in scheduler 
	 */
	cancel_delayed_work(&round_counter_work);
	/* remove two entries in /proc, called buffer and enabler */
	remove_proc_entry(PROCFS_BUFFER, NULL);
	remove_proc_entry(PROCFS_FILESUFFEX_SETTER, NULL);

	printk(KERN_INFO "Goodbye\n");
}
 
module_init(MemDataCollector_init);
module_exit(MemDataCollector_exit);
