#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chris Brumgard");
MODULE_DESCRIPTION("My Linux module.");
MODULE_VERSION("1.0");


static int __init simple_module_init(void) {
    printk(KERN_INFO "Hello, World!\n");
    return 0;
}

static void __exit simple_module_exit(void) {
    printk(KERN_INFO "Goodbye!\n");
}


module_init(simple_module_init);
module_exit(simple_module_exit);
