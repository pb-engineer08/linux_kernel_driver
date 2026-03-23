#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>


#define MYMAJOR 24
static int driver_open(struct inode *device_file, struct file *instance);
static int driver_close(struct inode *device_file, struct file *instance);

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = driver_open,
	.release = driver_close
};



/**
 * @brief This function is called, when the device file is opened
 */
static int driver_open(struct inode *device_file, struct file *instance) {
	printk("dev number - open was called!\n");
	return 0;
}

/**
 * @brief This function is called, when the device file is closed
 */
static int driver_close(struct inode *device_file, struct file *instance) {
	printk("dev number - close was called!\n");
	return 0;
}



/**
 * @brief This function is called, when the module is loaded into the kernel
 */

static int __init mod_init(void) {
	int retval;
	printk("init, Kernel!\n");
	/* register device number. */
	retval = register_chrdev(MYMAJOR, "device_number", &fops);
	if(retval == 0) {
		printk("dev number - registered Major: %d, Minor: %d\n", MYMAJOR, 0);
	}
	else if(retval > 0) {
		printk("dev number - registered Device number Major: %d, Minor: %d\n", retval>>20, retval&0xfffff);
	}
	else {
		printk("Could not register device number!\n");
		return -1;
	}
	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit mod_exit(void) {
	unregister_chrdev(MYMAJOR, "dev-number");
	printk("Deinit, Kernel\n");
}

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("PB Linux Drivers");
MODULE_DESCRIPTION("Registrations Device number  in kernel module");

module_init(mod_init);
module_exit(mod_exit);


