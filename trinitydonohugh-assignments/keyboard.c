#include "gpio.h"
#include "gpioextra.h"
#include "keyboard.h"
#include "ps2.h"
#include "timer.h"

static unsigned int CLK, DATA; 

void wait_for_falling_clock_edge(void) {
    while (gpio_read(CLK) == 0) {}
    while (gpio_read(CLK) == 1) {}
}

void keyboard_init(unsigned int clock_gpio, unsigned int data_gpio) 
{
    CLK = clock_gpio;
    gpio_set_input(CLK); 
    gpio_set_pullup(CLK); 
 
    DATA = data_gpio;
    gpio_set_input(DATA); 
    gpio_set_pullup(DATA); 
}

unsigned int readBit (){
    wait_for_falling_clock_edge();
    return gpio_read(DATA);
}

unsigned char keyboard_read_scancode(void) 
{
    unsigned char bitArray[8];
    unsigned int highCounter = 0;
    unsigned int clockCounter = 0;

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
    key_action_t action;
    unsigned char curScancode = keyboard_read_scancode();
    if (curScancode == 0xe0){
        curScancode = keyboard_read_scancode();
        if (curScancode != 0xf0) {
            action.what = KEY_PRESS;
            action.keycode = curScancode;
            return action;
        } else {
            action.what = KEY_RELEASE;
            curScancode = keyboard_read_scancode();
            action.keycode = curScancode;
            return action;
        }
    }
    if (curScancode != 0xf0) {
        action.what = KEY_PRESS;
        action.keycode = curScancode;
        return action;
    } 
    if (curScancode == 0xf0) {
        action.what = KEY_RELEASE;
        curScancode = keyboard_read_scancode();
        action.keycode = curScancode;
        return action;
    } 
}

void editEventModifiersHelper (key_event_t event, key_action_t action, unsigned int shiftBit) {
    enum { KEY_PRESS, KEY_RELEASE } actionWhat = action.what;

    if (actionWhat == KEY_PRESS) {
        event.modifiers = 1 << shiftBit;
    } else {
        event.modifiers = 0 << shiftBit;
    }
}

keyboard_modifiers_t modifiers = 0b0;

key_event_t keyboard_read_event(void) 
{
    while (1) {
        key_action_t action = keyboard_read_sequence();
        unsigned int actionKeycode = (unsigned int)action.keycode;

        key_event_t event;

        if (action.what == KEY_PRESS) {
            if (actionKeycode == 0x7e) {
                modifiers = modifiers ^ KEYBOARD_MOD_SCROLL_LOCK;
            } else if (actionKeycode == 0x77) {
                modifiers = modifiers ^ KEYBOARD_MOD_NUM_LOCK;
            } else if (actionKeycode == 0x58) {
                modifiers = modifiers ^ KEYBOARD_MOD_CAPS_LOCK;
            } else if (actionKeycode == 0x12 || actionKeycode == 0x59) {
                modifiers = modifiers | KEYBOARD_MOD_SHIFT;
            } else if (actionKeycode == 0x11) {
                modifiers = modifiers | KEYBOARD_MOD_ALT;
            } else if (actionKeycode == 0x14) {
                modifiers = modifiers | KEYBOARD_MOD_CTRL;
            }
        } else {
            if (actionKeycode == 0x12 || actionKeycode == 0x59) {
                modifiers = modifiers & ~KEYBOARD_MOD_SHIFT;
            } else if (actionKeycode == 0x11) {
                modifiers = modifiers & ~KEYBOARD_MOD_ALT;
            } else if (actionKeycode == 0x14) {
                modifiers = modifiers & ~KEYBOARD_MOD_CTRL;
            }
        }
            event.action = action;
            event.key = ps2_keys[actionKeycode];
            event.modifiers = modifiers;
            return event;
    }
}


unsigned char keyboard_read_next(void) 
{
    while (1) {
        key_event_t event = keyboard_read_event();
        if ((event.action.what == KEY_PRESS) && ((event.action.keycode >= 0) 
            || (event.action.keycode <= 83))) {
            if ((((event.modifiers) & KEYBOARD_MOD_NUM_LOCK) == KEYBOARD_MOD_NUM_LOCK) 
                && ((event.action.keycode >= 0x69 && event.action.keycode <= 0x75)
                || event.action.keycode == 0x7a || event.action.keycode == 0x7d)) {
                return event.key.other_ch;
            } else if ((((event.modifiers) & KEYBOARD_MOD_CAPS_LOCK) == KEYBOARD_MOD_CAPS_LOCK) 
                && ((event.action.keycode == 0x15) 
                || (event.action.keycode >= 0x1a && event.action.keycode <= 0x1d) 
                || (event.action.keycode >= 0x21 && event.action.keycode <= 0x24)
                || (event.action.keycode >= 0x2a && event.action.keycode <= 0x2d)
                || (event.action.keycode >= 0x31 && event.action.keycode <= 0x35)
                || (event.action.keycode == 0x3b) || (event.action.keycode == 0x3c)
                || (event.action.keycode == 0x3a) || (event.action.keycode == 0x42)
                || (event.action.keycode == 0x43) || (event.action.keycode == 0x44)
                || (event.action.keycode == 0x4b) || (event.action.keycode == 0x4d))) {
                return event.key.other_ch;
            } else if ((((event.modifiers & KEYBOARD_MOD_SHIFT) >> 3) == 1) 
                && (event.key.other_ch != 0)) {
                return event.key.other_ch;
            } else if (event.action.keycode != 0x58 && event.action.keycode != 0x59 
                && event.action.keycode != 0x77 && event.action.keycode != 0x12) {
                return event.key.ch;
            }
        }
    }
}
