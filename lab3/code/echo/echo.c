#include "uart.h"

int main ( void )
{
    uart_init();

    while(1) {        
        unsigned int ra=uart_getchar();
        if(ra==0x0D) ra = 0x0A;
        uart_putchar(ra);
    }

    return(0);
}
