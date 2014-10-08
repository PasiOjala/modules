/*                                                     
 * $Id: hello.c,v 1.5 2004/10/26 03:32:21 corbet Exp $ 
 */                                                    
#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");
#include<linux/moduleparam.h>
//Module parameters
static int delay =11; 

int delay_set(const char *val, const struct kernel_param *kp)
{
    long l;
    int ret=0;
    ret=kstrtoul(val,0,&l);
//    printk("delay now : %d\n",delay);
    if (ret<0||((int)l !=l))
            return ret<0?ret:-EINVAL;
    
    *((int *)kp->arg) = l; 
    printk("Hello, new delay: %d\n",l);
    return 0;
};

int delay_get (char *buffer, const struct kernel_param *kp)
{
    //return 0;
    printk("Hello, delay from get: %d\n",delay);
    return scnprintf(buffer, PAGE_SIZE,"%d", *((int *)kp->arg));
};
static struct kernel_param_ops my_ops={
    .get=delay_get,
    .set=delay_set
};

//module_param(delay, int, 0664);
module_param_cb(delay,&my_ops,&delay,0644);
static int hello_init(void)
{
	printk(KERN_ALERT "%d Hello, world\n",delay);
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, cruel world %d\n",delay);
}

module_init(hello_init);
module_exit(hello_exit);
