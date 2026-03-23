#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>

/* data to read write */
static char buffer[255];
static size_t buffer_pointer = 0;

/* global variables */
static dev_t device_number;
static struct class *device_struct;
static struct cdev rw_device;

static ssize_t device_driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs);
static ssize_t device_driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs);
static int device_driver_close(struct inode *device_file, struct file *instance);
static int device_driver_open(struct inode *device_file, struct file *instance);

#define DRIVER_NAME  "rw_driver"
#define DRIVER_CLASS "rw_device"

static struct file_operations fops = {.owner = THIS_MODULE,
    .open = device_driver_open,
    .release = device_driver_close,
    .read = device_driver_read,
    .write = device_driver_write};

/**
 * @brief output the read buffer
 */
static ssize_t device_driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
  int copy_byte, byte_not_copied, diff;

  /* Get number of bytes to copy */
  copy_byte = min(count, buffer_pointer);

  /* Copy byte to user */
  byte_not_copied = copy_to_user(user_buffer, buffer, copy_byte);

  /* Calculate byte */
  diff = copy_byte - byte_not_copied;

  return diff;
}

/**
 * @brief Write bytes to buffer
 */
static ssize_t device_driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
  int copy_byte, byte_not_copied, diff;

  /* Get number of bytes to copy */
  copy_byte = min(count, sizeof(buffer));

  /* Copy byte to user */
  byte_not_copied = copy_from_user(buffer, user_buffer, copy_byte);
  buffer_pointer = copy_byte;

  /* Calculate byte */
  diff = copy_byte - byte_not_copied;

  return diff;
}

/**
 * @brief Called, when the device file is opened  
 */
static int device_driver_open(struct inode *device_file, struct file *instance) {
  printk("open  called!\n");
  return 0;
}

/**
 * @brief called, when the device file is opened
 */
static int device_driver_close(struct inode *device_file, struct file *instance) {
  printk("close  called!\n");
  return 0;
}

/**
 * @brief called, when the module is loaded into the kernel
 */
static int __init mod_init(void) {
  /* assign a device nr */
  if (alloc_chrdev_region(&device_number, 0, 1, DRIVER_NAME) < 0) {
    printk("Device Number could not be assigned!\n");
    return -1;
  }
  printk("read_write driver - Device Number Major: %d, Minor: %d was assigned!\n", device_number >> 20,
      device_number & 0xfffff);

  /* Create device class */
  if ((device_struct = class_create(DRIVER_CLASS)) == NULL) {
    printk("Failed to create Device class!\n");
    goto ERR_ClASS;
  }

  /* create device file */
  if (device_create(device_struct, NULL, device_number, NULL, DRIVER_NAME) == NULL) {
    printk("Fail to create device file!\n");
    goto ERR_FILE;
  }

  /* init device file */
  cdev_init(&rw_device, &fops);

  /* registering device driver to kernel */
  if (cdev_add(&rw_device, device_number, 1) == -1) {
    printk("Registering of device to kernel failed!\n");
    goto ERR_DEVICE_ADD;
  }

  return 0;
ERR_DEVICE_ADD:
  device_destroy(device_struct, device_number);
ERR_FILE:
  class_destroy(device_struct);
ERR_ClASS:
  unregister_chrdev_region(device_number, 1);
  return -1;
}

/**
 * @brief Called, when the module is removed from the kernel
 */
static void __exit mod_deinit(void) {
  cdev_del(&rw_device);
  device_destroy(device_struct, device_number);
  class_destroy(device_struct);
  unregister_chrdev_region(device_number, 1);
  printk("Deinit, Kernel Module\n");
}

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("PB Linux Drivers");
MODULE_DESCRIPTION("Device read or write operation");

module_init(mod_init);
module_exit(mod_deinit);
