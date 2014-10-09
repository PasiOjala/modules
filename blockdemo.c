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
#include<linux/gpio.h>
//Module parameters
//static int delay =11; 

//module params
static struct class * my_class;
static dev_t my_devnum;
static struct cdev my_cdev;
static struct device *my_device;
#define BUFSIZE 256
static char buf[BUFSIZE];


/************************************
GPIO
*/
static int gpiobase=240;
module_param(gpiobase,int,0444);
#define SELECT (gpiobase+0)
#define RIGHT (gpiobase+1)
#define DOWN (gpiobase+1)
#define UP (gpiobase+3)
#define LEFT (gpiobase+4)
//...
#define RED (gpiobase+6)
#define GREEN (gpiobase+7)
#define BLUE (gpiobase+8)

#define C_IN(pin) gpio_request_one(pin,GPIOF_DIR_IN,#pin)
#define C_OUTH(pin) gpio_request_one(pin,GPIOF_OUT_INIT_HIGH,#pin)
#define C_OUTL(pin) gpio_request_one(pin,GPIOF_OUT_INIT_LOW,#pin)
#define UC(pin) gpio_free(pin)


static void my_gpio_init(void){
    //gpio.txt doc at /linux
//int    gpio_request_one(SELECT, GPIOF_DIR_IN , "SELECT");

    C_IN(SELECT);
    C_IN(UP);
    C_IN(DOWN);
    C_IN(RIGHT);
    C_IN(LEFT);
    C_OUTH(RED);
    C_OUTH(GREEN);
    C_OUTH(BLUE);
    
}
static void my_gpio_exit(void){
    //gpio.txt doc at /linux
//int    gpio_request_one(SELECT, GPIOF_DIR_IN , "SELECT");

    UC(SELECT);
    UC(UP);
    UC(DOWN);
    UC(RIGHT);
    UC(LEFT);
    UC(RED);
    UC(GREEN);
    UC(BLUE);
    
}

static void set_on(char col){
    int pin =RED;
    if (col=='g') pin=GREEN;
    if (col=='b') pin=BLUE;
    
    gpio_set_value(pin,0);

}
static void set_off(char col){
    int pin =RED;
    if (col=='g') pin=GREEN;
    if (col=='b') pin=BLUE;
    
    gpio_set_value(pin,1);

}

static void process_buffer(void){
    
    int index=0;
    char color=0;
    
    while(buf[index]){
    
    switch(buf[index])
    {
        case 'r':
        case 'R':
            color='r';
        break;
        case 'g':
        case 'G':
            color='g';
        break;
        case 'b':
        case 'B':
            color='b';
        break;
        case '0':
            if(color){
            set_off(color);
            color=0;}
        break;
        case '1':
            set_on(color);
            color=0;
        break;
        default: 
            color=0;
    }
    index++;
    }
    memset(buf,0,BUFSIZE);
}

static DEFINE_SEMAPHORE(ioctlsem);
// ei näinstatic size_t buflen;
//static struct file_operations my_fileops;

static ssize_t my_read(struct file *filp, char __user *ubuff, size_t len, loff_t *offs){
    int remaining;
    printk(KERN_ALERT "block my_read\n");
    if (*offs+len>BUFSIZE)
        len=BUFSIZE-*offs;
    if (len<0) len=0;
    if  (!access_ok(VERIFY_WRITE,ubuff,len)){
        return -EFAULT  ;
}

    remaining=copy_to_user(ubuff,buf,len);
    if (remaining){
        return -EFAULT  ;
    }
    //optional
    *offs+=len;

    
    return len;
}

static ssize_t my_write(struct file *filp, const char __user *ubuff, size_t len, loff_t *offs) {
int remaining;
printk(KERN_ALERT "block my_write\n");
if (len>BUFSIZE)
    len=BUFSIZE;
if  (!access_ok(VERIFY_READ,ubuff,len)){
    return -EFAULT  ;
}

remaining=copy_from_user(buf,ubuff,len);
if (remaining){
    return -EFAULT  ;
}

process_buffer();
// ei näin buflen=len;
//optional
*offs+=len;

return len;
}

static int my_open(struct inode *my_inode, struct file *filp){
printk(KERN_ALERT "block open\n");
return 0;}

static int my_release(struct inode * my_inode, struct file *filp) {
printk(KERN_ALERT "block my_release\n");

return 0;}

//static long my_locked_ioctl(struct file *filp, unsigned int cmd, unsigned long data)
//{
//    down(&ioctlsem);
//printk(KERN_ALERT "block LOCKED ioctl "
//        "cmd 0x%x  data 0x%lx\n",cmd,data);
//    up(&ioctlsem);
//return 0;
//}
static long my_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long data)
{

printk(KERN_ALERT "block unlocked ioctl "
        "cmd 0x%x  data 0x%lx\n",cmd,data);

return 0;
}

struct file_operations my_fileops = {
    .read =     my_read,
    .write =    my_write,
    .open =     my_open,
    .release =  my_release,
    .unlocked_ioctl=my_unlocked_ioctl,
//    .locked_ioctl=my_locked_ioctl,
};

static int hello_init(void)
{
    int err=0;
    
    printk(KERN_ALERT "Hello, block world\n");
    //1: create class
    my_class= class_create(THIS_MODULE,"blockdemoclass");
    if (IS_ERR(my_class))
        goto class_create_error;

    //2: alloc chardev number
    err=alloc_chrdev_region(&my_devnum, 0, 1, "blockdemochreg");
    if (err){
        printk(KERN_ALERT "FAILEDDevice number reserved %d:%d\n",
        MAJOR(my_devnum), MINOR(my_devnum));
        goto chardev_reg_error;
    }
    printk(KERN_ALERT "Device number reserved %d:%d\n",
    MAJOR(my_devnum), MINOR(my_devnum));

    //3: init cdev
    cdev_init(&my_cdev, &my_fileops);
    my_cdev.owner = THIS_MODULE;
    //4:add cdev
    err=cdev_add(&my_cdev,my_devnum,1);
    if(err){
            printk(KERN_ALERT "Device add failed, %d\n",err);
            goto cdev_add_error;
    }
    //5: create device
    my_device=device_create(my_class,NULL,my_devnum,NULL,"blockdevice");
    if (IS_ERR(my_device))
        goto device_create_error;
    
    my_gpio_init();
    return 0;

device_create_error:
    cdev_del(&my_cdev);
cdev_add_error:    
    unregister_chrdev_region(my_devnum, 1);    
chardev_reg_error:
    class_destroy(my_class);
class_create_error:
    class_destroy(my_class);
    return err;
}

static void hello_exit(void)
{
    my_gpio_exit();
    device_destroy(my_class,my_devnum);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_devnum, 1);
    class_destroy(my_class);
	printk(KERN_ALERT "Goodbye, block world \n");
}

module_init(hello_init);
module_exit(hello_exit);
