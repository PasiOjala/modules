/* 
 * File:   ioctltest.c
 * Author: ely
 *
 * Created on 7. lokakuuta 2014, 14:59
 */

#include <stdio.h>
#include <stdlib.h>
#include <stropts.h>
/*
 * 
 */
int main(int argc, char** argv) {

    FILE *f = fopen("/dev/blockdevice","r+");
    ioctl(fileno(f),0x12121212,0x34343434);
    fclose(f);
    return 0;
}

