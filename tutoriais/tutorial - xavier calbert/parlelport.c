#include<linux/init.h>
//#include<linux/config.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/slab.h>
#include<linux/fs.h>
#include<linux/errno.h>
#include<linux/types.h>
#include<linux/proc_fs.h>
#include<linux/fcntl.h>
#include<linux/ioport.h>

int parlelport_open(struct inode *inode , struct file *filp);
int parlelport_release(struct inode *inode , struct file *filp);
ssize_t parlelport_read(struct file *filp, const char *buf, size_t count , loff_t *f_pos);
ssize_t parlelport_write(struct file *filp, const char *buf, size_t count , loff_t *f_pos);

void parlelport_exit(void);
int parlelport_init(void);

struct file_operations parlelport_fops = {
    read : parlelport_read,
    write : parlelport_write,
    open : parlelport_open,
    release : parlelport_release
};

int parlelport_major = 61;
int port;

int parlelport_init(void){
	int result;
	result = register_chrdev(parlelport_major, "parlelport", &parlelport_fops);
	if(result  < 0){
		printk("<1> cannot obtain major number %d\n", parlelport_major);
		return result;
	}
	parlelport_buffer = kmalloc(1, GFP_KERNEL);
	if(!parlelport_buffer){
		result = -ENOMEM;
		goto fail;
	}
    /*registering */
    port  = check_region(0x378 , 1);
    if(port){
        printk("<1> parlelport canot reserve 0x378\n");
        result = port;
        goto fail;
    }
    request_region(0x378 , 1,"parlelport");
	printk("<1> inserting memory moduel\n");
	return 0;
	fail :
		parlelport_exit();
		return result;
}

void memory_exit(void){
	unregister_chrdev(parlelport_major, "parlelport");
	/* exit*/
    if(!port){
        release_region(0x378 , 1);
    }
	printk("<1> removing parlelport module\n");
}

int parlelport_open(struct inode * inode, struct file *filp){
	return 0;
}

int parlelport_release(struct inode * inode, struct file *filp){
	return 0;
}

ssize_t parlelport_read(struct file *filp, char *buf, size_t count, loff_t *f_pos){
    char parlelport_buffer;
    /*reading */
    parlelport_buffer = inb(0x378);
    copy_to_user(buf, parlelport_buffer,1);	
    if(*f_pos == 0){
		*f_pos+=1;
		return 1;
	}else{
		return 0;
	}
}

ssize_t parlelport_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos){
	char *tmp;
	tmp = buf+count - 1;
	copy_from_user(memory_buffer, tmp,1);
    outb(parlelport_buffer, 0x378);
	return 1;
}

module_init(parlelport_init);
module_exit(parlelport_exit);










