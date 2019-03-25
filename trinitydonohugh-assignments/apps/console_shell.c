#include "gpio.h"
#include "keyboard.h"
#include "console.h"
#include "shell.h"

// Main function for assignment 6 (console)
// You should not need to modify this.
// It should boot right into the graphical version of your shell
// from assignment 5.

void main(void)
{
    gpio_init();
    console_init(20, 40);
    keyboard_init(KEYBOARD_CLOCK, KEYBOARD_DATA);

    shell_init(console_printf);
    shell_run();
}
