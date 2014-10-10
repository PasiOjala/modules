#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  int ret=0;
  if (argc < 2) {
    fprintf(stderr, "Need an arg to write\n");
    return 1;
  }

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
    
    
    printf("wrote %s\n", argv[1]);
    //fseek(f, 0, SEEK_SET);
    //fread(buffer, len+1, 1, f);
    fclose(f);
        return ret;

}

