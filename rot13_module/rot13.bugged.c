#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/ctype.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Chris Brumgard");
MODULE_DESCRIPTION("My Linux module.");
MODULE_VERSION("1.0");

#define DEVICE_NAME "rot13"
#define BUFFER_SIZE 4096

/* Device major number */
static int major;

/* Static buffer */
static char rot13_buf[BUFFER_SIZE] = {0};
static int read_idx  = 0;
static int write_idx = 0;

/*** Device callbacks ***/
static int dev_open(struct inode *inodep, struct file *filep) {

   printk(KERN_INFO "Rot13 device opened\n");
   
   return 0;
}

static int dev_release(struct inode *inodep, struct file *filep) {

   printk(KERN_INFO "Rot13 device closed\n");

   return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {

    int rc = 0;
    
    len = (write_idx-read_idx < len) ? write_idx-read_idx: len;


    /* Copy the buffer to user space */
    rc = copy_to_user(buffer, rot13_buf+read_idx, len);

    read_idx += len;

    return (rc == 0) ? len : -EFAULT;
}

static ssize_t dev_write(struct file *filep, const char *buffer,
                         size_t len, loff_t *offset) {

   int rc = 0;
   int i;

   /* Copy the buffer to kernel space */
   rc = copy_from_user(rot13_buf+write_idx, buffer, len);

   if(rc != 0) {
	return -EFAULT;
   } 

   for(i=0; i<len; write_idx++) {
	rot13_buf[write_idx] = (tolower(rot13_buf[write_idx]) - 'a' + 13) % 26 + 'a';
   }

   return len;
}


/* Define the device callbacks in the file operations struct */
static struct file_operations fops = {
   .owner = THIS_MODULE,
   .open = dev_open,
   .read = dev_read,
   .write = dev_write,
   .release = dev_release,
};



static int __init rot13_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);

    if (major < 0) {
        printk(KERN_ALERT "Unable to register character device\n");
        return major;
    }

    printk(KERN_INFO "Rot13 module has been loaded: %d\n", major);

    memset(rot13_buf, 0, sizeof(rot13_buf));
    return 0;
}

static void __exit rot13_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "Rot13 module has been unloaded\n");
}

module_init(rot13_init);
module_exit(rot13_exit);
