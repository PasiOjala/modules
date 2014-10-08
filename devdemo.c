/*                                                     
 * $Id: hello.c,v 1.5 2004/10/26 03:32:21 corbet Exp $ 
 */                                                    
#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");
#include<linux/moduleparam.h>
#include<linux/device.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
//Module parameters
//static int delay =11; 

//module params
static struct class * my_class;
static dev_t my_devnum;
static struct cdev my_cdev;
static struct device *my_device;
#define BUFSIZE 256
static char buf[BUFSIZE];
// ei näinstatic size_t buflen;
//static struct file_operations my_fileops;

static ssize_t my_read(struct file *filp, char __user *ubuff, size_t len, loff_t *offs){
    int remaining;
    printk(KERN_ALERT "my_read\n");
    if (*offs+len>BUFSIZE)
        len=BUFSIZE-*offs;
    if (len<0) len=0;
    if  (!access_ok(VERIFY_WRITE,ubuff,len)){
        return -EFAULT  ;
}

    remaining=copy_to_user(ubuff,buf,len);// ei näinmin(buflen,len));
    if (remaining){
        return -EFAULT  ;
    }
    //optional
    *offs+=len;

    
    return len;
}

static ssize_t my_write(struct file *filp, const char __user *ubuff, size_t len, loff_t *offs) {
int remaining;
printk(KERN_ALERT "my_write\n"); 
if (len>BUFSIZE)
    len=BUFSIZE;
if  (!access_ok(VERIFY_READ,ubuff,len)){
    return -EFAULT  ;
}

remaining=copy_from_user(buf,ubuff,len);
if (remaining){
    return -EFAULT  ;
}
// ei näin buflen=len;
//optional
*offs+=len;

return len;
}

static int my_open(struct inode *my_inode, struct file *filp){
printk(KERN_ALERT "open\n");
return 0;}

static int my_release(struct inode * my_inode, struct file *filp) {
printk(KERN_ALERT "my_release\n");
return 0;}

struct file_operations my_fileops = {
    .read =     my_read,
    .write =    my_write,
    .open =     my_open,
    .release =  my_release,
};


static int hello_init(void)
{
    int err;
    printk(KERN_ALERT "Hello, dev world\n");
    //1: create class
    my_class= class_create(THIS_MODULE,"devdemoclass");
    // alloc chardev number
    err=alloc_chrdev_region(&my_devnum, 0, 1, "devdemochreg");
    if (err){
        printk(KERN_ALERT "FAILEDDevice number reserved %d:%d\n",
        MAJOR(my_devnum), MINOR(my_devnum));
    }
    printk(KERN_ALERT "Device number reserved %d:%d\n",
    MAJOR(my_devnum), MINOR(my_devnum));

    //3: init cdev
    cdev_init(&my_cdev, &my_fileops);
    my_cdev.owner = THIS_MODULE;
    //4:add cdev
    cdev_add(&my_cdev,my_devnum,1);
    
    //5: create device
    my_device=device_create(my_class,NULL,my_devnum,NULL,"devdemodevice");
    
    return err;
}

static void hello_exit(void)
{
    device_destroy(my_class,my_devnum);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_devnum, 1);
    class_destroy(my_class);
	printk(KERN_ALERT "Goodbye, DEV world \n");
}

module_init(hello_init);
module_exit(hello_exit);
