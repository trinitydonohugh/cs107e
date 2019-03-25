#include "gpio.h"
#include "gpioextra.h"
#include "uart.h"
#include "interrupts.h"
#include "ringbuffer.h"
#include "printf.h"
#include "assert.h"

static const int BUTTON = GPIO_PIN25;
// static volatile int previous_cnt = 0;
// static volatile cnt = 0;

static rb_t *rb;

void button_init(void)
{   
    gpio_set_input(BUTTON);
    gpio_set_pullup(BUTTON);

    gpio_set_input()
}

static bool button_press(unsigned int pc) 
{
    static int cnt = 0;
    if (gpio_check_and_clear_event(BUTTON)) {
        rb_enqueue(rb, cnt);
        cnt++;
        return true;
    }
    return false;
}

void setup_interrupts(void) 
{
    gpio_enable_event_detection(BUTTON, GPIO_DETECT_FALLING_EDGE);
    interrupts_attach_handler(button_press);
    interrupts_enable_source(INTERRUPTS_GPIO3);
    interrupts_global_enable();
}

void wait_for_falling_clock_edge(void) {
    while (gpio_read(BUTTON) == 0) {}
    while (gpio_read(BUTTON) == 1) {}
}

void wait_for_click(void)
{
    wait_for_falling_clock_edge(); 
    printf("+");
}

void main(void) 
{
    rb = rb_new();

    gpio_init();
    uart_init();
    button_init();
    setup_interrupts();

    int count;
    while (1) {
        if (!rb_empty(rb)) {
            rb_dequeue(rb, &count);
            printf("%d\n", count);
        } 
    }
}
