
#include <linux/init.h>
#include <linux/module.h>


#include<linux/device.h>
#include<linux/fs.h>
#include<linux/cdev.h>
#include<linux/uaccess.h>
#include<linux/ctype.h>

MODULE_LICENSE("Dual BSD/GPL");
int chmod(const char*, mode_t);

//Module parameters

#define BUF_MAX_SIZE 80
//static char lcd_buffer[BUF_MAX_SIZE] = "abcdefghijklmnop";
static char lcd_buffer[BUF_MAX_SIZE];// = "abc\nefghij\nm";

#define ROWS 4
static int columns = 5;
static int rows = ROWS;

int bufcols = BUF_MAX_SIZE/ROWS;

int virt_lcd_param_nrows_set(const char *val, const struct kernel_param *kp) {
    long l;
    int ret = 0;
    ret = kstrtoul(val, 0, &l);
    //    printk("delay now : %d\n",delay);
    if (ret < 0 || ((int) l != l) || ((l != 1) && (l != 2) && (l != 4)) || (l * columns > BUF_MAX_SIZE))
        return ret < 0 ? ret : -EINVAL;

    *((int *) kp->arg) = l;
    rows = l;
    
       switch (rows){
   case 1:
       if (columns==16) {
           bufcols=BUF_MAX_SIZE/2;
       }
       else bufcols = BUF_MAX_SIZE;
       break;
   case 2:
       bufcols = BUF_MAX_SIZE/2;
       break;
   case 4:
       bufcols = BUF_MAX_SIZE/4;
       break;
   default:
       printk(KERN_ALERT "shouldn't be here\n");
       
   };
    return 0;
};

int virt_lcd_param_rlen_set(const char *val, const struct kernel_param *kp) {
    long l;
    int ret = 0;
    ret = kstrtoul(val, 0, &l);
    //    printk("delay now : %d\n",delay);
    if (ret < 0 || ((int) l != l) || (l * rows > BUF_MAX_SIZE) || l < 1)
        return ret < 0 ? ret : -EINVAL;

    *((int *) kp->arg) = l;
    columns = l;
    return 0;
};

int virt_lcd_param_get(char *buffer, const struct kernel_param *kp) {

    return scnprintf(buffer, PAGE_SIZE, "%d", *((int *) kp->arg));

};

int virt_lcd_buffer_get(char *buffer, const struct kernel_param *kp) {
    int c = 0;
    int r = 0;
    /*
        char *temp;
     */

    while (r < rows) {
        c = 0;
        while (c < columns) {
            //buffer[r * columns + c + r] = ((char*) kp->arg)[r * columns + c];
            buffer[r * columns + c + r] = ((char*) kp->arg)[r * bufcols + c];
            c++;
        }
        buffer[r * columns + c + r] = '\n';
        r++;
    }
    
    buffer[rows*(columns+1) ] = '\0';
    return rows * (columns + 1) +1/*the null*/;

};
static struct kernel_param_ops virt_lcd_buffer_ops = {
    //    .flags=KERNEL_PARAM_FL_NOARG,

    .get = virt_lcd_buffer_get,
};
static struct kernel_param_ops virt_lcd_param_rlen_ops = {
    //    .flags=KERNEL_PARAM_FL_NOARG,
    .get = virt_lcd_param_get,
    .set = virt_lcd_param_rlen_set

};
static struct kernel_param_ops virt_lcd_param_nrows_ops = {
    //    .flags=KERNEL_PARAM_FL_NOARG,
    .get = virt_lcd_param_get,
    .set = virt_lcd_param_nrows_set

};

module_param_cb(columns, &virt_lcd_param_rlen_ops, &columns, S_IRUGO | S_IWUSR | S_IWGRP);
module_param_cb(rows, &virt_lcd_param_nrows_ops, &rows, S_IRUGO | S_IWUSR | S_IWGRP);
module_param_cb(lcd_buffer_contents, &virt_lcd_buffer_ops, lcd_buffer, S_IRUGO);
/*********************/
/*device*/
static struct class * my_lcd_class;
static dev_t my_devnum;
static struct cdev my_cdev;
static struct device *my_device;

static char buf[BUF_MAX_SIZE];
// ei näinstatic size_t buflen;
//static struct file_operations my_fileops;

//#define EMPTY ' '
#define EMPTY '.'
static loff_t readpos=0;

static ssize_t my_read(struct file *filp, char __user *ubuff, size_t len, loff_t *offs) {
    int remaining;
    int eof=0;
     printk(KERN_ALERT "my_read, filp->file_pos %d. *offs %d, len %d\n", (int)filp->f_pos, (int)*offs,len);
   
    
    
    if (*offs>=BUF_MAX_SIZE){
        printk(KERN_ALERT "returning 0 from read\n");
        return 0;}
        

    
    if (*offs+ len > BUF_MAX_SIZE){
        len = BUF_MAX_SIZE - *offs;
        
    }
    if (len < 0) len = 0;
    if (!access_ok(VERIFY_WRITE, ubuff, len)) {
        return -EFAULT;
    }

    remaining = copy_to_user(ubuff, buf+*offs, len); 
/*
    if (eof){
        copy_to_user(ubuff+len,'\0',1);
    }
*/
    if (remaining) {
        return -EFAULT;
    }
    //optional
     
    *offs += len;
    printk(KERN_ALERT "returning:::, filp->file_pos %d. *offs %d, len %d\n", (int)filp->f_pos, (int)*offs,len);
    //return len+eof;
    return len;
}

//check start of buffer for csi code, return length of code, exluding the escape
//modify input parameters to match possible code.

int check_csi(const char* buf, int* n, int* m, char* letter) {
    int csi_len = 0;
    int next_num = 0;

    if (buf[csi_len] == '[') {

        while (isdigit(buf[++csi_len])) {
            next_num = next_num * 10 + (buf[csi_len] &0xf);
        }
        *n = next_num;
        if (buf[csi_len] == ';') {
            next_num = 0;
            while (isdigit(buf[++csi_len])) {
                next_num = next_num * 10 + (buf[csi_len]&0xf);
            }
            *m = next_num;
            if (isalpha(buf[csi_len])) {
                *letter = buf[csi_len++];
            }
        }
        else if (isalpha(buf[csi_len])) {
            *letter = buf[csi_len++];
        }
    }
    return csi_len; 
}

static ssize_t my_write(struct file *filp, const char __user *ubuff, size_t len, loff_t *offs) {
    int remaining;
    int ibuff= 0;
    int itempbuff=0;
    int csi_n, csi_m, csi_len;
    char csi_letter;
    int ibuffsaver=0;
    
    char tempbuf[2*BUF_MAX_SIZE]; //allow for one control per one char msx

    if (len > 2*BUF_MAX_SIZE)
        len = 2*BUF_MAX_SIZE;

    if (!access_ok(VERIFY_READ, ubuff, len)) {
        return -EFAULT;
    }
    //get tempbuf from user side
   remaining = copy_from_user(tempbuf, ubuff, len);
    if (remaining) {
        printk("remaining %d\n",remaining);
        return -EFAULT;
    }
   

   
   //copy data from tempbuff to actual buffer memory
   //in desired format
    for (itempbuff = 0; itempbuff < len; itempbuff++)
    {
        if (tempbuf[itempbuff] == '\\')
        {
            itempbuff++;
            switch (tempbuf[itempbuff])
            {
            case 'n':
                do
                {
                 buf[ibuff++] = EMPTY;
                 }
                //while ((ibuff % columns != 0) && (ibuff<BUF_MAX_SIZE));
                while ((ibuff % bufcols!= 0) && (ibuff<BUF_MAX_SIZE));
                break;
            case 'e':
            {
                csi_len = check_csi(&tempbuf[itempbuff+1], &csi_n, &csi_m,&csi_letter);
                itempbuff+=csi_len;
                ibuffsaver=ibuff;
                
                switch (csi_letter)
                {
                case 'J':
                    switch (csi_n)
                    {

                    case 0:

                        for (; ibuff < BUF_MAX_SIZE; ibuff++)
                        {
                            buf[ibuff] =EMPTY;
                        }
                        ibuff = ibuffsaver;
                        break;
                    case 1:
                        for (; ibuff >= 0; ibuff--)
                        {
                            buf[ibuff] = EMPTY;
                        }
                        ibuff = ibuffsaver;
                        break;
                    case 2:
                        for (ibuff = 0; ibuff < BUF_MAX_SIZE; ibuff++)
                        {
                            buf[ibuff] =EMPTY;
                        }
                        ibuff = ibuffsaver;
                        break;
                    default:
                        ;
                    };
                    break;
                case 'H':
                    //ok so why does not returning -EINVAL cause an error
                    if (csi_n<1) csi_n=1;
                    if (csi_n>rows)csi_n=rows;
                    if (csi_m<1) csi_m=1;
                    if (csi_m>columns) csi_m=columns;
                    
                    ibuff=(csi_n-1)*columns+csi_m-1;
                    break;
                default:
 //                   printk("<1> got letter %c\n", csi_letter);
                    break;
                }

            }
            default:
                ;
            };

        }
        else
            if (tempbuf[itempbuff]!='\0'){
            buf[ibuff++] = tempbuf[itempbuff];
            if (ibuff%columns==0){
                ibuff+=bufcols-columns;
            }
            }
    }
    strncpy(lcd_buffer,buf,BUF_MAX_SIZE);
    //optional
    *offs += len;
    return len;
}

static int my_open(struct inode *my_inode, struct file *filp) {
    printk(KERN_ALERT "open\n");
    return 0;
}

static int my_release(struct inode * my_inode, struct file *filp) {
    printk(KERN_ALERT "my_release\n");
    return 0;
}

static loff_t my_llseek(struct file *filp, loff_t off, int whence)
{
        //struct device *dev = filp->private_data;
        loff_t newpos;

        printk ("SEEK: off = %d, whence= %d, fpos %d\n",(int)off, whence, (int)filp->f_pos);
        switch(whence) {
          case 0: /* SEEK_SET */
                newpos = off;
                break;

          case 1: /* SEEK_CUR */
                newpos = filp->f_pos + off;
                break;

          case 2: /* SEEK_END */
                newpos = BUF_MAX_SIZE+ off;
                break;

          default: /* can't happen */
                return -EINVAL;
        }
        if (newpos < 0) return -EINVAL;
        if (newpos > BUF_MAX_SIZE) newpos=BUF_MAX_SIZE;
        filp->f_pos = newpos;
        printk ("SEEK:;;; return newpos%d\n",newpos);
        return newpos;
}


struct file_operations my_fileops = {
    .read = my_read,
    .write = my_write,
    .open = my_open,
    .release = my_release,
    .llseek = my_llseek,
};

static int virt_lcd_init(void) {
    int err;
    printk(KERN_ALERT "Hello, LCD world\n");
    /*
                    chmod("/sys/module/virtlcd/parameters/rows",S_IWUGO);
     */

    printk(KERN_ALERT "Hello, dev world\n");
    //1: create class
    my_lcd_class = class_create(THIS_MODULE, "lcdclass");
    // alloc chardev number
    err = alloc_chrdev_region(&my_devnum, 0, 1, "lcdchreg");
    if (err) {
        printk(KERN_ALERT "FAILEDDevice number reserved %d:%d\n",
                MAJOR(my_devnum), MINOR(my_devnum));
    }
    printk(KERN_ALERT "Device number reserved %d:%d\n",
            MAJOR(my_devnum), MINOR(my_devnum));

    //3: init cdev
    cdev_init(&my_cdev, &my_fileops);
    my_cdev.owner = THIS_MODULE;
    //4:add cdev
    cdev_add(&my_cdev, my_devnum, 1);

    //5: create device
    my_device = device_create(my_lcd_class, NULL, my_devnum, NULL, "lcddevice");
    //init buff full of spaces
    for (err=0;err<BUF_MAX_SIZE;err++){
        buf[err]=EMPTY;
    }
    return 0;
}

static void virt_lcd_exit(void) {
    device_destroy(my_lcd_class, my_devnum);
    cdev_del(&my_cdev);
    unregister_chrdev_region(my_devnum, 1);
    class_destroy(my_lcd_class);
    printk(KERN_ALERT "BYE, LCD world\n");
}

module_init(virt_lcd_init);
module_exit(virt_lcd_exit);