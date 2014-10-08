//#include<stdio.h>
//
//int main(){
////    char* read;
////    puts("yayay\n");
//    int rc='\0';
//    setvbuf(stdin,NULL,_IONBF,1);
//    while (rc!='.')
//    {
////        rc=getc(stdin);
////        putc(rc,stdout);
//        rc=getchar();
//        
//        putchar(rc);//        puts("yayay\n");
//        fflush(stdout);
//        
//        //        printf("%c\n",rc);
//    }
//    printf("\ndone\n");
//    return 0;
//}
#include <stdio.h>
#include <termio.h>

static struct termio savemodes;
static int havemodes = 0;

int tty_break()
{
	struct termio modmodes;
	if(ioctl(fileno(stdin), TCGETA, &savemodes) < 0)
		return -1;
	havemodes = 1;
	modmodes = savemodes;
	modmodes.c_lflag &= ~ICANON;
	modmodes.c_cc[VMIN] = 1;
	modmodes.c_cc[VTIME] = 0;
	return ioctl(fileno(stdin), TCSETAW, &modmodes);
}

int tty_getchar()
{
	return getchar();
}

int tty_fix()
{
	if(!havemodes)
		return 0;
	return ioctl(fileno(stdin), TCSETAW, &savemodes);
}
int main(){
//    char* read;
//    puts("yayay\n");
    int rc='\0';
//    setvbuf(stdin,NULL,_IONBF,1);
    tty_break();
    while (rc!='.')
    {
//        rc=getc(stdin);
//        putc(rc,stdout);
        rc=tty_getchar();
        
//        putchar(rc);//        puts("yayay\n");
//        fflush(stdout);
//        
        //        printf("%c\n",rc);
    }
    printf("\ndone\n");
    return 0;
}
