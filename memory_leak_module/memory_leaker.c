#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chris Brumgard");
MODULE_DESCRIPTION("My Linux module.");
MODULE_VERSION("1.0");


static char *buf = NULL;

static int __init memory_leaker_init(void) {
    printk(KERN_INFO "Memory leader loaded\n");

    buf = kmalloc(4*1024*1024, GFP_KERNEL);
    
    return 0;
}

static void __exit memory_leaker_exit(void) {
    printk(KERN_INFO "Memory leaker unloaded!\n");

}


module_init(memory_leaker_init);
module_exit(memory_leaker_exit);
