        /*                                                     
 * $Id: hello.c,v 1.5 2004/10/26 03:32:21 corbet Exp $ 
 */                                                                              
#include <linux/init.h>
#include <linux/module.h>
MODULE_LICENSE("Dual BSD/GPL");



//Module parameters

#define BUF_MAX_SIZE 80
static char lcd_buffer[BUF_MAX_SIZE]="0123456789   34 ";

static int char_per_row=4;
static int lcd_rows=4;



int virt_lcd_param_nrows_set(const char *val, const struct kernel_param *kp)
{
    long l;
    int ret=0;
    ret=kstrtoul(val,0,&l);
//    printk("delay now : %d\n",delay);
    if (ret<0||((int)l !=l) || ((l!=1)&&(l!=2)&&(l!=4)) || (l*char_per_row>BUF_MAX_SIZE))
            return ret<0?ret:-EINVAL;
    
    *((int *)kp->arg) = l; 
    lcd_rows=l;
    return 0;
};
int virt_lcd_param_rlen_set(const char *val, const struct kernel_param *kp)
{
    long l;
    int ret=0;
    ret=kstrtoul(val,0,&l);
//    printk("delay now : %d\n",delay);
    if (ret<0||((int)l !=l) || (l*lcd_rows>BUF_MAX_SIZE))
            return ret<0?ret:-EINVAL;
    
    *((int *)kp->arg) = l; 
    char_per_row=l;
    return 0;
};
int virt_lcd_param_get(char *buffer, const struct kernel_param *kp)
{
    
    return scnprintf(buffer, PAGE_SIZE,"%d", *((int *)kp->arg));
  
};
int virt_lcd_buffer_get(char *buffer, const struct kernel_param *kp)
{
    int c=0;
    int r=0;
/*
    char *temp;
*/

    while (r<lcd_rows){
    c=0;
        while (c<char_per_row){

            /*
            if (i>0 && i%lcd)
    */
    /*
            printk(KERN_ALERT"adding %c\n",((char* )kp->arg )[i]);
    */
            buffer[r*char_per_row +c+r]=((char*)kp->arg )[r*char_per_row +c];
            printk(KERN_ALERT "added %c, c was %d, r was %d\n",buffer[r*char_per_row +c+r],c,r);
            c++;

        }
    buffer[r*char_per_row +c+r]='\n';
    printk(KERN_ALERT "adding newline,r was %d, c was %d\n",r,c);
    r++;
    }
    buffer[(r+1)*char_per_row ]='\0';
     printk(KERN_ALERT "put null at %d",(r+1)*char_per_row);

    
/*
    len=scnprintf(temp, BUF_MAX_SIZE+1,"%s", *((char* )kp->arg ));
*/
/*
    return scnprintf(buffer, PAGE_SIZE,"%s", *((char* )kp->arg ));
*/
     printk(KERN_ALERT "returning %d\n",lcd_rows*char_per_row+1/*the null*/+lcd_rows);
    return lcd_rows*char_per_row+1/*the null*/+lcd_rows/*the newlines*/;
  
};
static struct kernel_param_ops virt_lcd_buffer_ops={
//    .flags=KERNEL_PARAM_FL_NOARG,

    .get=virt_lcd_buffer_get, 
/*    .set=virt_lcd_param_set
*/
};
static struct kernel_param_ops virt_lcd_param_rlen_ops={
//    .flags=KERNEL_PARAM_FL_NOARG,

    .get=virt_lcd_param_get, 
    .set=virt_lcd_param_rlen_set

};
static struct kernel_param_ops virt_lcd_param_nrows_ops={
//    .flags=KERNEL_PARAM_FL_NOARG,

    .get=virt_lcd_param_get, 
    .set=virt_lcd_param_nrows_set

};

module_param_cb(rowsize, &virt_lcd_param_rlen_ops,&char_per_row, S_IRUGO|S_IWUSR);
module_param_cb(rows, &virt_lcd_param_nrows_ops,&lcd_rows, S_IRUGO|S_IWUSR);
module_param_cb(lcd_buffer_contents, &virt_lcd_buffer_ops,lcd_buffer, S_IRUGO);


static int virt_lcd_init(void)
{
	printk(KERN_ALERT "Hello, LCD world\n");
	return 0;
}

static void virt_lcd_exit(void)
{
	printk(KERN_ALERT "BYE, LCD world\n");
}

module_init(virt_lcd_init);
module_exit(virt_lcd_exit);