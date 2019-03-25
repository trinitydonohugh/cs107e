#include "gpio.h"
#include "gpioextra.h"
#include "keyboard.h"
#include "ps2.h"

static unsigned int CLK, DATA;

void keyboard_init(unsigned int clock_gpio, unsigned int data_gpio) 
{
    CLK = clock_gpio;
    gpio_set_input(CLK);
    gpio_set_pullup(CLK); 
 
    DATA = data_gpio;
    gpio_set_input(DATA);
    gpio_set_pullup(DATA); 
}

static void wait_for_falling_clock_edge(void) 
{
    while (gpio_read(CLK) == 0) {}
    while (gpio_read(CLK) == 1) {}
}

unsigned int readBit (){
    wait_for_falling_clock_edge();
    return gpio_read(DATA);
}

unsigned char keyboard_read_scancode(void) 
{
    unsigned char bitArray[8];
    unsigned int highCounter = 0;

    while (1) {
        if (readBit() == 1) {
            continue;
        }
        for (int i = 0; i < 8; i++) {
            bitArray[i] = (char)readBit();
            if (bitArray[i] == 1) {
                highCounter++;
            }
        }
        unsigned int parityBit = readBit();
        if ((parityBit == 1 && highCounter&1) || (parityBit == 0 && !highCounter&1)) {
            continue;
        }
        if (readBit() == 0) {
            continue;
        }
        unsigned char bit = 0;
        for (int i = 0; i < 8; i++) {
            bit += (bitArray[i] << i);
        }
        return bit;
    }
}

key_action_t keyboard_read_sequence(void)
{
    // The implementation started below assumes a sequence is exactly
    // one byte long. Although this is  the case for ordinary key press 
    // actions, it is not true for all key actions.
    // What key actions generate a sequence of length 2?  What
    // about length 3?
    // Figure out what those cases are and extend this code to
    // properly handle those sequences and generate the correct
    // key actions
    key_action_t action;
    action.keycode = keyboard_read_scancode();
    action.what = KEY_PRESS;
    return action;
}
