/*                                                     
 * $Id: hello.c,v 1.5 2004/10/26 03:32:21 corbet Exp $ 
 */                                                    
#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");
#include<linux/moduleparam.h>
//Module parameters
static int a =1; 
static int b =1; 
static int tulo =1; 
static int kerrat =0;
static int hoohoo;

int ab_set(const char *val, const struct kernel_param *kp)
{
        long l;
    int ret=0;
    ret=kstrtoul(val,0,&l);
//    printk("delay now : %d\n",delay);
    if (ret<0||((int)l !=l))
            return ret<0?ret:-EINVAL;
    
    *((int *)kp->arg) = l; 
    tulo=a*b;
    printk("ab set, a %d b %d tulo%d",a,b,tulo);
    return 0;
};
int ab_get (char *buffer, const struct kernel_param *kp)
{
    
    return scnprintf(buffer, PAGE_SIZE,"%d", *((int *)kp->arg));
  
};

int tulo_set(const char *val, const struct kernel_param *kp)
{
    return 0;
};

int tulo_get (char *buffer, const struct kernel_param *kp)
{
    //return 0;
    kerrat++;
    return scnprintf(buffer, PAGE_SIZE,"%d", *((int *)kp->arg));
};
//static struct kernel_param_ops my_ops={
//    .get=tulo_get,
//    .set=tulo_set
//};
static struct kernel_param_ops ab_ops={
//    .flags=KERNEL_PARAM_FL_NOARG,
    .get=ab_get,
    .set=ab_set
};
static struct kernel_param_ops tulo_ops={
    .get=tulo_get,
    .set=tulo_set
};
module_param_cb(a, &ab_ops,&a, S_IRUGO|S_IWUSR);
module_param_cb(bertta, &ab_ops,&b, S_IRUGO|S_IWUSR);
//module_param_cb(a, &ab_ops,NULL, S_IRUGO|S_IWUSR);
//module_param_cb(b, &ab_ops,NULL, S_IRUGO|S_IWUSR);



module_param_cb(tulo, &tulo_ops,&tulo, S_IRUGO);
module_param(kerrat, int, S_IRUGO);

//module_param(a, int, S_IRUGO|S_IWUSR);
//module_param(b, int, S_IRUGO|S_IWUSR);
//module_param(tulo, int, S_IRUGO);
//module_param(kerrat, int, S_IRUGO S_IWUSR);
//module_param_cb(delay,&my_ops,&delay,0644);
static int hello_init(void)
{
	printk(KERN_ALERT "%d Hello, world\n",kerrat);
	return 0;
}

static void hello_exit(void)
{
	printk(KERN_ALERT "Goodbye, cruel world %d\n",kerrat);
}

module_init(hello_init);
module_exit(hello_exit);
