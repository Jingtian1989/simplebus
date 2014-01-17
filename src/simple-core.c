#include "simple.h"

static struct simple_device *simple_dev_get(struct simple_device *dev)
{
	if (dev)
		get_device(&dev->device);
	return dev;
}

static void simple_dev_put(struct simple_device *dev)
{
	if (dev)
		put_device(&dev->device);

}


static int simple_device_match(struct device *dev, struct device_driver *drv)
{
	struct simple_device *sim_dev = to_simple_device(dev);
	struct simple_driver *sim_drv = to_simple_driver(drv);
	struct simple_class_id *ids = sim_drv->classes;

	if (ids)
		while (ids->class_id != SIMPLE_CLASS_END)
		{
			if (ids->class_id == sim_dev->class_id.class_id)
				return 1;
			ids++;
		}
	return 0;
}



static int simple_device_probe(struct device *dev)
{
	int ret = 0;
	struct simple_device *sim_dev;
	struct simple_driver *sim_drv;
	sim_dev = to_simple_device(dev);
	sim_drv = to_simple_driver(dev->driver);
	simple_dev_get(sim_dev);
	if (sim_drv->probe)
		ret = sim_drv->probe(sim_dev);
	if (ret)
		simple_dev_put(sim_dev);
	return ret;
}


static int simple_device_remove(struct device *dev)
{
	struct simple_device *sim_dev = to_simple_device(dev);
	struct simple_driver *sim_drv = to_simple_driver(dev->driver);
	if (sim_drv)
	{
		if (sim_drv->remove)
		{
			sim_drv->remove(sim_dev);
		}
	}

	simple_dev_put(sim_dev);
	return 0;
}


static ssize_t simple_device_show_class_id(struct device *dev, struct device_attribute *attr, char *buf)
{
	struct simple_device *sim_dev = to_simple_device(dev);
	sprintf(buf, "0x%x\n", sim_dev->class_id.class_id);
	return strlen(buf) + 1;
}

struct device_attribute simple_device_attrs[] = 
{
	__ATTR(class_id, S_IRUGO, simple_device_show_class_id, NULL),
	__ATTR_NULL
};

struct bus_type simple_bus_type = 
{
	.name 		= "simple",
	.match 		= simple_device_match,
	.probe 		= simple_device_probe,
	.remove 	= simple_device_remove,
	.dev_attrs 	= simple_device_attrs 
};

struct simple_bus simple_bus;

static void simple_bus_release(struct device *dev)
{
	return;
}

static int __init simple_init(void)
{
    int ret;
    /* create /sys/bus/simple.*/
    if ((ret = bus_register(&simple_bus_type)) < 0)
    {
    	printk(KERN_ALERT "[SIMPLE] bus_register failed.\n");
    	goto err_bus_register;
    }
    simple_bus.dev.release = simple_bus_release;
    dev_set_name(&simple_bus.dev, "simple");
    /* create /sys/devices/simple.*/
    if ((ret = device_register(&simple_bus.dev)))
    {
    	printk(KERN_ALERT "[SIMPLE] device_register failed.\n");
    	goto err_dev_register;
    }
    return ret;
err_dev_register:
	bus_unregister(&simple_bus_type);
err_bus_register:
	return ret;
}

static void __exit simple_exit(void)
{
	device_unregister(&simple_bus.dev);
	bus_unregister(&simple_bus_type);
}

static void simple_device_release(struct device *dev)
{
	return;
}


int simple_register_device(struct simple_device *dev)
{
	dev_set_name(&dev->device, dev->name);
	dev->device.bus 	= &simple_bus_type;
	dev->device.parent 	= &simple_bus.dev; 
	dev->device.release = simple_device_release;
	return device_register(&dev->device);
}
EXPORT_SYMBOL(simple_register_device);


void simple_unregister_device(struct simple_device *dev)
{
	/* 
	 *	device_unregister will do in two parts, first, remove it from all the subsystems with device_del(), 
	 *	then decrement the reference count via put_device(). If that is the final reference count,
	 *	the device will be cleaned up via device_release() above. Otherwise, the structure will stick 
	 *	around until the final reference to the device is dropped.
	 */
	device_unregister(&dev->device);
} 
EXPORT_SYMBOL(simple_unregister_device);



int simple_register_driver(struct simple_driver *drv)
{
	int ret = 0;
	drv->driver.name= drv->name;
	drv->driver.bus = &simple_bus_type;
	ret = driver_register(&drv->driver);
	return ret;
}
EXPORT_SYMBOL(simple_register_driver);

void simple_unregister_driver(struct simple_driver *drv)
{
	driver_unregister(&drv->driver);
}
EXPORT_SYMBOL(simple_unregister_driver);



module_init(simple_init);
module_exit(simple_exit);
MODULE_LICENSE("GPL");