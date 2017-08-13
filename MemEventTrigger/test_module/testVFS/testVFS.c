#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/version.h>

#include <linux/vmstat.h>
#include <linux/mm.h>

#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>


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

static int __init NoifyMem_init(void)
{
	struct file *file = NULL;
	char txt[20]="hi i'm a receiver";


	file = file_open("/proc/receiver", O_RDWR | O_APPEND | O_CREAT, 0644);
	
	if (IS_ERR(file)) 
	{
		printk("error occured while opening file /proc/receiver, exiting...\n");
		return 0;
	}

	file_write(file,file->f_pos,txt,20);
	file_close(file);

	return 0;
}

 
static void __exit NoifyMem_exit(void)
{
	printk(KERN_INFO "Goodbye\n");
}
 
module_init(NoifyMem_init);
module_exit(NoifyMem_exit);
