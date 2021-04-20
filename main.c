#include <stdio.h>
#include "uart.h"


int main(void)
{
    uart_init();

    while(1)
    {
        printf("main loop\n");
        sleep(1);

    }
 

}