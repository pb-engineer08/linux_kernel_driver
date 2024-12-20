#ifndef GPIO_OPERATIONS_H
#define GPIO_OPERATIONS_H

#include <linux/fs.h>

/* Function prototypes for file operations */
ssize_t driver_read(struct file *File, char *user_buffer, size_t count, loff_t *offs);
ssize_t driver_write(struct file *File, const char *user_buffer, size_t count, loff_t *offs);
int driver_open(struct inode *device_file, struct file *instance);
int driver_close(struct inode *device_file, struct file *instance);

#endif // GPIO_OPERATIONS_H
