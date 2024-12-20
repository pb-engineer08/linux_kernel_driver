#ifndef GPIO_DRIVER_H
#define GPIO_DRIVER_H

#include <linux/fs.h>

/* Meta Information */
#define DRIVER_NAME "my_gpio_driver"
#define DRIVER_CLASS "MyModuleClass"

/* Function prototypes */
int gpio_driver_init(void);
void gpio_driver_exit(void);

#endif // GPIO_DRIVER_H
