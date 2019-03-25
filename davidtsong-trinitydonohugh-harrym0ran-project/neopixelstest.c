#include "gpio.h"
#include "printf.h"
#include "strings.h"
#include "stdbool.h"
#include "timer.h"
#include "interrupts.h"
#include "neopixels.h"

void main(void) {
	neopixels_init(5);
	neopixels_sendBit(true);
	
}