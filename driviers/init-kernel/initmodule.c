#include <linux/init.h>
#include <linux/module.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("PB Linux Drivers");
MODULE_DESCRIPTION("Initial Kernel Module");

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init kernel_init(void) {
  printk("Kernel Initailized !\n");
  return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit kernel_exit(void) {
  printk("Kernel Deinitialized \n");
}

module_init(kernel_init);
module_exit(kernel_exit);
