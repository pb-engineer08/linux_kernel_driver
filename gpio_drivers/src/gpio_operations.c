#include <linux/uaccess.h>
#include <linux/gpio.h>
#include <linux/kernel.h>

#include "inc/gpio_operations.h"

ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;
    char tmp[3] = " \n";

    /* Get amount of data to copy */
    to_copy = min(count, sizeof(tmp));

    /* Read value of interrupt button */
    tmp[0] = gpio_get_value(17) + '0';

    /* Copy data to user */
    not_copied = copy_to_user(user_buffer, &tmp, to_copy);

    delta = to_copy - not_copied;
    return delta;
}

ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs) {
    int to_copy, not_copied, delta;
    char value;

    /* Get amount of data to copy */
    to_copy = min(count, sizeof(value));

    /* Copy data from user */
    not_copied = copy_from_user(&value, user_buffer, to_copy);

    /* Set GPIO output based on input */
    switch (value) {
        case '0':
            gpio_set_value(4, 0);
            break;
        case '1':
            gpio_set_value(4, 1);
            break;
        default:
            printk("Invalid Input!\n");
            break;
    }

    delta = to_copy - not_copied;
    return delta;
}

int driver_open(struct inode *device_file, struct file *instance) {
    printk("Device file opened!\n");
    return 0;
}

int driver_close(struct inode *device_file, struct file *instance) {
    printk("Device file closed!\n");
    return 0;
}
