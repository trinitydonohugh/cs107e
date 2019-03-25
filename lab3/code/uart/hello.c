#include "timer.h"
#include "uart.h"

void putchar(int c)
{
    uart_putchar(c);
}

void putstring(char *s)
{
    while(*s)
        putchar(*s++);
}

void main (void)
{
    uart_init();
    while(1) {
        putstring( "hello, laptop\n" );
        timer_delay(1);
    }
}
