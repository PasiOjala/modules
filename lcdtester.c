#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int ret=0;
  if (argc < 2) {
    fprintf(stderr, "Need an arg to write\n");
    return 1;
  }

  char readbuffer[81];
  int len = strlen(argv[1]);
  char *buffer = (char *)malloc(len+1);

  if (!buffer) { 
    fprintf(stderr, "No memory\n");
    return 2;
  }

  int count = 0;
  
  buffer[len]='\0';

  
    FILE *f = fopen("/dev/lcddevice", "w+");
    if (!f) return -1;
    //fseek(f, 0, SEEK_SET);
    ret=fwrite(argv[1], len, 1, f);
            printf("printing returned code %d\n",ret);
    
    
    //printf("wrote %s\n", argv[1]);
    //fseek(f, 0, SEEK_SET);
    //fread(buffer, len+1, 1, f);
    fclose(f);
    
    
    f = fopen("/dev/lcddevice", "r");
    if (!f) return -1;
    //fseek(f, 0, SEEK_SET);
    ret=fread(readbuffer, 80, 1, f);
            printf("read returned code %d\n",ret);
            readbuffer[80]=0;
    
    printf("read from devfs:\n%s\n", readbuffer);
    fclose(f);
    
       f = fopen("/sys/module/virtlcd/parameters/lcd_buffer_contents", "r");
    if (!f) return -1;
    //fseek(f, 0, SEEK_SET);
    ret=fread(readbuffer, 80, 1, f);
            printf("read returned code %d\n",ret);
    
    printf("read from sysfs\n%s\n", readbuffer);
    fclose(f);

    #define RC 4
        f = fopen("/dev/lcddevice", "r");
    if (!f) return -1;
    
            ret=fread(readbuffer,RC, 1, f);
            printf("read returned code %d\n",ret);
            readbuffer[RC]=0;
    
    printf("read from devfs 4 bytes:\n%s\n", readbuffer);  
    ret=fread(readbuffer,RC, 1, f);
            printf("read returned code %d\n",ret);
            readbuffer[RC]=0;
    
    printf("read from devfs 4 bytes:\n%s\n", readbuffer);
    
   fseek(f, 1,SEEK_SET);

    ret=fread(readbuffer,RC, 1, f);
            printf("read returned code %d\n",ret);
            readbuffer[RC]=0;
    
    printf("read from devfs W/ SEEK_SET 1 :\n%s\n", readbuffer);
    
    
    
    fseek(f, 20, SEEK_SET);
        ret=fread(readbuffer,RC, 1, f);
            printf("read returned code %d\n",ret);
            readbuffer[RC]=0;
    printf("read from devfs W/SEEK_SET 20:\n%s\n", readbuffer);
   
    //seek cur shows up as 0 at kernel?????????

        fseek(f, 20-RC, SEEK_CUR);
        ret=fread(readbuffer,RC, 1, f);
            printf("read returned code %d\n",ret);
            readbuffer[RC]=0;
    
    printf("read from devfs W/20-RC, SEEK_CUR 20-RC :\n%s\n", readbuffer);

            
    
    
    fseek(f, 0-20, SEEK_END);
        ret=fread(readbuffer,RC, 1, f);
            printf("read returned code %d\n",ret);
            readbuffer[RC]=0;
    
    printf("read from devfs W/1-20, SEEK_END :\n%s\n", readbuffer);
    
    
        ret=fread(readbuffer,20, 1, f);
            printf("read returned code %d\n",ret);
            readbuffer[20]=0;
    
    printf("read from devfs 20bytes, goes overboard :\n%s\n", readbuffer);
    
    
    char  newreadbuffer[21];
    fseek(f,-16,SEEK_END);
        ret=fread(newreadbuffer,20, 1, f);
            printf("read returned code %d\n",ret);
            newreadbuffer[20]=0;
    
    printf("read from devfs 20bytes to new readbuffer, goes overboard :\n%s\n", newreadbuffer);
    
    
    fclose(f);
    
    
    
    return ret;

}

