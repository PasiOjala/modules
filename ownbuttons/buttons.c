/* 
 * File:   buttons.c
 * Author: user
 *
 * Created on October 12, 2014, 1:43 PM
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/*
 * 
 */

int do_logic(int* red, int*green, int input){
    
    switch(input){
    case 1:
        *red^=1;
        *green=0;
        break;
        
        case 2:
        *green^=1;
        *red=0;
        break;
        default:
            printf("invalid input\n");
            return -1;
    }
    return 0;
}

inline void printstate(int red, int green){
    printf ("RED: %d, GREEN: %d\n",red,green);
}

int main(int argc, char** argv)
{
    int red=0;
    int green = 0;
    int i=0, input=0;
    
    srand(time(NULL));
    printstate(red,green);
    
    for (i=0;i<15;i++){
        input=rand()%2+1;
        printf("input %d -> ",input);
        if (do_logic(&red,&green,input)) {
            return -1;
        }
        printstate(red,green);
    }

    
    return (EXIT_SUCCESS);
}

