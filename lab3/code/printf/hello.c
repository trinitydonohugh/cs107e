#include "timer.h"
#include "uart.h"
#include "printf.h"

void main (void)
{
    uart_init();
    timer_delay(2);
	unsigned int *FSEL2 = (unsigned int*) 0x20200008;
    while(1) {
        printf( "hello, laptop\n" );
        timer_delay(1);
        printf("value: %d\n", *FSEL2);
        gpio_set_function(20, 1);
        gpio_set_function(21, 1);
        printf("value: %d\n", *FSEL2);
    }
}
