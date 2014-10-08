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
static int read_index=0;
static int write_index=0;
// ei näinstatic size_t buflen;
//static struct file_operations my_fileops;

static ssize_t my_read(struct file *filp, char __user *ubuff, size_t len, loff_t *offs){
    int remaining;
    int available;
    available=write_index-read_index;
    printk(KERN_ALERT "my_fifo_read, len %d available %d\n",len,available);
    available=write_index-read_index;
    if (available<0) available+=BUFSIZE;
    if (len>available) len=available;
    if  (!access_ok(VERIFY_WRITE,ubuff,len)){
        return -EFAULT  ;
}

    if (read_index+len<BUFSIZE){
            remaining=copy_to_user(ubuff,buf+read_index,len);
            read_index+=len;
    }
    else {
        remaining=copy_to_user(ubuff,buf+read_index,BUFSIZE-read_index);
        
        read_index=remaining;
        remaining=copy_to_user(ubuff,buf,remaining);

    }
    if (remaining){
        return -EFAULT  ;
    }
    return len;
}

static ssize_t my_write(struct file *filp, const char __user *ubuff, size_t len, loff_t *offs) {
int remaining;
int available;
available=BUFSIZE-write_index+read_index;

printk(KERN_ALERT "my_writelen %d available %d\n",len,available);
if (len>BUFSIZE)
    len=BUFSIZE;
if  (!access_ok(VERIFY_READ,ubuff,len)){
    return -EFAULT  ;
}

if (write_index+len<BUFSIZE){
    remaining=copy_from_user(buf+write_index,ubuff,len);
    write_index+=len;
}
else
{
    remaining=copy_from_user(buf+write_index,ubuff,len-write_index);
    write_index=remaining;
    remaining=copy_from_user(ubuff,buf,remaining);
    
}
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



//int delay_set(const char *val, const struct kernel_param *kp)
//{
//    long l;
//    int ret=0;
//    ret=kstrtoul(val,0,&l);
////    printk("delay now : %d\n",delay);
//    if (ret<0||((int)l !=l))
//            return ret<0?ret:-EINVAL;
//    
//    *((int *)kp->arg) = l; 
//    printk("Hello, new delay: %ld\n",l);
////    printk("Hello, DEL=: %d\n",delay);
//    
//    return 0;
//};
//
//int delay_get (char *buffer, const struct kernel_param *kp)
//{
//    //return 0;
////    printk("Hello, delay from get: %d\n",++delay);
//    return scnprintf(buffer, PAGE_SIZE,"%d", *((int *)kp->arg));
//};
////static struct kernel_param_ops my_ops={
////    .get=delay_get,
////    .set=delay_set
////};
//
////module_param(delay, int, 0664);
////module_param_cb(delay,&my_ops,&delay,0644);


static int hello_init(void)
{
    int err;
    printk(KERN_ALERT "Hello, dev world\n");
    //1: create class
    my_class= class_create(THIS_MODULE,"fifodemoclass");
    // alloc chardev number
    err=alloc_chrdev_region(&my_devnum, 0, 1, "fifodemochreg");
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
    my_device=device_create(my_class,NULL,my_devnum,NULL,"fifodemodevice");
    
    return err;
}

static void hello_exit(void)
{
    device_destroy(my_class,my_devnum);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_devnum, 1);
    class_destroy(my_class);
	printk(KERN_ALERT "Goodbye, fifo world \n");
}

module_init(hello_init);
module_exit(hello_exit);
