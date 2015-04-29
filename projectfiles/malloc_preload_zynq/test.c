#include <stdio.h>
#include <stdlib.h>

int main()
{
  int i = 0;
    printf("Hello World!\n");

    volatile char* foo = malloc(sizeof(char)*100);

    char* cp = "Hello Ian!";
    printf("foo: 0x%x\n", foo);

    if(foo == NULL)
    {
        printf("MALLOC FAILED!\n");
    }
    else
    {
        printf("MALLOC SUCCEEDED!\n");
    }
    
    *foo++ = 0x11;
    *foo++ = 0x22;
    *foo++ = 0x33;
    *foo++ = 0x44;
    *foo++ = 0x55;
    *foo++ = 0x66;
    *foo++ = 0x77;
    *foo++ = 0x88;
    *foo++ = 0x99;
    *foo++ = 0xAA;
    *foo++ = 0xBB;
    *foo++ = 0xCC;
    *foo++ = 0xDD;
    *foo++ = 0xEE;
    *foo++ = 0xFF;

   
    //    free(foo);
    return 0;
}
