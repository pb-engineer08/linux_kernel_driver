#include "gpio_driver.h"
#include "gpio_operations.h"
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/gpio.h>
#include <linux/uaccess.h>

static dev_t device_number;
static struct class *gpio_class;
static struct cdev gpio_device;

struct cleanup_step {
    void (*cleanup_fn)(void); // Function pointer for cleanup
    const char *description;  // Description for debugging
};

/* Cleanup functions */
static void unregister_device(void) {
    unregister_chrdev_region(device_number, 1);
    printk("Device number unregistered.\n");
}

static void destroy_class(void) {
    if (gpio_class) {
        class_destroy(gpio_class);
        printk("Device class destroyed.\n");
    }
}

static void destroy_device(void) {
    if (gpio_class && device_number) {
        device_destroy(gpio_class, device_number);
        printk("Device destroyed.\n");
    }
}

static void remove_cdev(void) {
    cdev_del(&gpio_device);
    printk("Character device removed.\n");
}

static void free_gpio_17(void) {
    gpio_free(17);
    printk("GPIO 17 freed.\n");
}

static void free_gpio_4(void) {
    gpio_free(4);
    printk("GPIO 4 freed.\n");
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = driver_open,
    .release = driver_close,
    .read = driver_read,
    .write = driver_write
};

int gpio_driver_init(void) {
    struct cleanup_step cleanup_steps[] = {
        { free_gpio_17, "Free GPIO 17" },
        { free_gpio_4, "Free GPIO 4" },
        { remove_cdev, "Remove cdev" },
        { destroy_device, "Destroy device" },
        { destroy_class, "Destroy class" },
        { unregister_device, "Unregister device" },
    };
    size_t cleanup_index = 0;

    printk("Hello, Kernel!\n");

    /* Allocate device number */
    if (alloc_chrdev_region(&device_number, 0, 1, DRIVER_NAME) < 0) {
        printk("Device number allocation failed.\n");
        return -1;
    }

    /* Create device class */
    if ((gpio_class = class_create(THIS_MODULE, DRIVER_CLASS)) == NULL) {
        printk("Device class creation failed.\n");
        goto error;
    }

    /* Create device file */
    if (device_create(gpio_class, NULL, device_number, NULL, DRIVER_NAME) == NULL) {
        printk("Device creation failed.\n");
        cleanup_index = 1; // Prepare for "Destroy class"
        goto error;
    }

    /* Initialize cdev and add to kernel */
    cdev_init(&gpio_device, &fops);
    if (cdev_add(&gpio_device, device_number, 1) == -1) {
        printk("Character device addition failed.\n");
        cleanup_index = 2; // Prepare for "Destroy device"
        goto error;
    }

    /* GPIO setup */
    if (gpio_request(4, "rpi-gpio-4") || gpio_direction_output(4, 0)) {
        printk("GPIO 4 setup failed.\n");
        cleanup_index = 3; // Prepare for "Remove cdev"
        goto error;
    }

    if (gpio_request(17, "rpi-gpio-17") || gpio_direction_input(17)) {
        printk("GPIO 17 setup failed.\n");
        cleanup_index = 4; // Prepare for "Free GPIO 4"
        goto error;
    }

    return 0;

error:
    while (cleanup_index < sizeof(cleanup_steps) / sizeof(cleanup_steps[0])) {
        if (cleanup_steps[cleanup_index].cleanup_fn) {
            cleanup_steps[cleanup_index].cleanup_fn();
        }
        cleanup_index++;
    }
    return -1;
}

void gpio_driver_exit(void) {
    gpio_set_value(4, 0);
    gpio_free(17);
    gpio_free(4);
    cdev_del(&gpio_device);
    device_destroy(gpio_class, device_number);
    class_destroy(gpio_class);
    unregister_chrdev_region(device_number, 1);
    printk("Driver removed!\n");
}

module_init(gpio_driver_init);
module_exit(gpio_driver_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("PB");
MODULE_DESCRIPTION("GPIO Driver");
