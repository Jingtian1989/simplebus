#include <linux/init.h>
#include <linux/module.h>
#include <linux/device.h>

#ifndef _SIMPLE_BUS_H_
#define _SIMPLE_BUS_H_

#define to_simple_device(dev) container_of(dev, struct simple_device, device)
#define to_simple_driver(drv) container_of(drv, struct simple_driver, driver)
#define SIMPLE_CLASS_END  	0xffff



struct simple_class_id
{
	u16 class_id;
};

struct simple_device
{
	char *name;
	struct device device;
	struct simple_class_id class_id;
};

struct simple_driver
{
	char *name;
	struct device_driver driver;
	struct simple_class_id *classes;
	int (*probe) (struct simple_device *dev);
	/*Device removed (NULL if not a hot-plug capable driver) */
	int (*remove) (struct simple_device *dev);
};


struct simple_bus
{
	struct device dev;
};

extern int simple_register_device(struct simple_device *dev);
extern void simple_unregister_device(struct simple_device *dev);
extern int simple_register_driver(struct simple_driver *drv);
extern void simple_unregister_driver(struct simple_driver *drv);

#endif