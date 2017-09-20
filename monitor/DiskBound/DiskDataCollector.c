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
#include <linux/proc_fs.h>

#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>
#include <linux/semaphore.h>
#include <xen/cagma.h>

#define collect_period 3000

#define PROCFS_MAXSIZE 512
#define PROC_ENABLE_RUN "enabler"
#define PROCFS1_NAME "buffer"

#define MainProg "accDisk"

static void _noify_to_procss_(struct work_struct *);
static DECLARE_DELAYED_WORK(_noify_to_procss_work,_noify_to_procss_);


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


static void _noify_to_procss_(struct work_struct *ws)
{
	
	if (enable_to_begin)
		round++;

	schedule_delayed_work(&_noify_to_procss_work,collect_period);
	
}


/*********************************************************
 *                  ProcFS section Beginning
 *********************************************************/

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
	
	
	sprintf(filepath,"/root/result/IO_RTIME.%d",fileNum);
	sprintf(context,"%d\t%s\n",round,procbuffer);
	_file_ = file_open(filepath, O_RDWR | O_APPEND | O_CREAT, 0644);
	
  	
	down(&sem);
		file_write(_file_,_file_->f_pos,context,strlen(context)+1);		
	up(&sem);
			
	file_close(_file_);
		
		
   	return count;
}

static ssize_t _handling_notification_(struct file *file, const char *buffer, size_t count,loff_t *data)
{
	char fileNumTemp[10];

	if(count > PROCFS_MAXSIZE)
       		count = PROCFS_MAXSIZE; 
   	if (copy_from_user(fileNumTemp,buffer,count))    
       		return -EFAULT;
	
	sscanf(fileNumTemp,"%d",&fileNum);
	
	schedule_delayed_work(&_noify_to_procss_work,0);

	return 0;
}

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


static int __init DiskDataCollector_init(void)
{
	
	create_buffer();
	create_enabler();

	return 0;
}

 
static void __exit DiskDataCollector_exit(void)
{

	cancel_delayed_work(&_noify_to_procss_work);
	remove_proc_entry(PROCFS1_NAME, NULL);
	remove_proc_entry(PROC_ENABLE_RUN, NULL);
	printk(KERN_INFO "Goodbye\n");
}
 
module_init(DiskDataCollector_init);
module_exit(DiskDataCollector_exit);
