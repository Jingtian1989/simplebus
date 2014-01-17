#include <linux/init.h>
#include <linux/module.h>
#include "simple.h"

int probe (struct simple_device *dev)
{
	printk(KERN_ALERT "simple_device probe.\n");
	return 0;
}

struct simple_device device = 
{
	.name = "simpledev",
	.class_id = {.class_id = 0x10},
};

struct simple_class_id class_id[] = 
{
	{.class_id = 0x10},
	{.class_id = SIMPLE_CLASS_END}
};

struct simple_driver driver = {
	.name = "simpledrv",
	.classes = class_id,
	.probe = probe,
};


static int __init simple_init(void)
{
	int ret = 0;
    if ((ret = simple_register_device(&device)))
    {
    	printk(KERN_ALERT "simple_register_device failed.\n");
    	goto err_reg_dev;
    }
    if ((ret = simple_register_driver(&driver)))
    {
    	printk(KERN_ALERT "simple_register_driver failed.\n");
    	goto err_reg_drv;
    }
	return ret;
err_reg_drv:
	simple_unregister_device(&device);
err_reg_dev:
	return ret;
}

static void __exit simple_exit(void)
{
	simple_unregister_driver(&driver);
	simple_unregister_device(&device);
}
module_init(simple_init);
module_exit(simple_exit);
MODULE_LICENSE("GPL");