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
#define PROC_ENABLE_RUN "enabler"
#define PROCFS1_NAME "buffer"
#define MainProg "FixedAccMem"

/* declare the template of task */
static void _notify_to_procss_(struct work_struct *);
static DECLARE_DELAYED_WORK(_notify_to_procss_work,_notify_to_procss_);

MODULE_DESCRIPTION("Noify meminfo");
MODULE_AUTHOR("Orion <sslouis25@icloud.com>");
MODULE_LICENSE("GPL");

static char procbuffer[PROCFS_MAXSIZE];

static bool enable_to_begin = 0;
static int fileNum;
static int round = 0;
//static int count_signal = 0;



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
static void _notify_to_procss_(struct work_struct *ws)
{

	if(enable_to_begin)
		round++;
	schedule_delayed_work(&_notify_to_procss_work,collect_period);
	
}



/*********************************************************
 *                  ProcFS section Beginning 
 *********************************************************/

/* set a callback of /proc/buffer to handle collecting the data from each memory-bound task */
static ssize_t buffer_write(struct file *file, const char *buffer, size_t count,loff_t *data)
{
 	struct semaphore sem; 
	struct file *_file_ = NULL;
	char filepath[30],context[20];

	sema_init(&sem,1);


	if(!enable_to_begin)
	{
		round = 1;
		enable_to_begin = 1;
	}

	if(count > PROCFS_MAXSIZE)
       		count = PROCFS_MAXSIZE; 
   	if (copy_from_user(procbuffer,buffer,count))    
       		return -EFAULT;
	
	
	sprintf(filepath,"/root/result/MEM_RTIME.%d",fileNum);
	sprintf(context,"%d\t%s\n",round,procbuffer);
	_file_ = file_open(filepath, O_RDWR | O_APPEND | O_CREAT, 0644);
	
  	
	down(&sem);
		file_write(_file_,_file_->f_pos,context,strlen(context)+1);		
	up(&sem);
			
	file_close(_file_);
		
		
   	return count;
}
/* 
 * set a callback function of /proc/enabler. when reading or writing it,
 * the system call this function to activate Data Collector 
 */
static ssize_t _handling_notification_(struct file *file, const char *buffer, size_t count,loff_t *data)
{
	char fileNumTemp[10];

	if(count > PROCFS_MAXSIZE)
       		count = PROCFS_MAXSIZE; 
   	if (copy_from_user(fileNumTemp,buffer,count))    
       		return -EFAULT;
	
	sscanf(fileNumTemp,"%d",&fileNum);
	
	schedule_delayed_work(&_notify_to_procss_work,0);

	return 0;
}
/* Create /proc/enabler in /proc to receive a command, which activate Data Collector */
static int create_enabler(void)
{
	static struct proc_dir_entry *p;
	static const struct file_operations proc_file_fops = {
		.owner = THIS_MODULE,
		.write = _handling_notification_,
	};

	p = proc_create(PROC_ENABLE_RUN, S_IRWXU, NULL, &proc_file_fops);

	if (!p) {
		printk("%s(#%d): create proc entry failed\n", __func__, __LINE__);        
		return -EFAULT;
	}

	return 0;

}

/* Create entry /proc/buffer in /proc to collect the data from each memory-bound task */
/* This is a part of Data Collecter */
static int create_buffer(void)
{
	static struct proc_dir_entry *p;
	static const struct file_operations proc_file_fops = {
		.owner = THIS_MODULE,
		.write = buffer_write,
	};    
 
   	p = proc_create(PROCFS1_NAME, S_IRWXU, NULL, &proc_file_fops);
        
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
static int __init ProcNoify_init(void)
{
	
	/* create two entries in /proc, called buffer and enabler */
	create_buffer();
	create_enabler();
	
	return 0;
}

 
/* this function to be called at module removeal time */ 
static void __exit ProcNoify_exit(void)
{

	/* remove all dalayable generated from template _notify_to_process_work	in scheduler */
	cancel_delayed_work(&_notify_to_procss_work);
	/* remove two entries in /proc, called buffer and enabler */
	remove_proc_entry(PROCFS1_NAME, NULL);
	remove_proc_entry(PROC_ENABLE_RUN, NULL);

	printk(KERN_INFO "Goodbye\n");
}
 
module_init(ProcNoify_init);
module_exit(ProcNoify_exit);