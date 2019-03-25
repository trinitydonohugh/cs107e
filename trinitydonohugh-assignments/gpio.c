#include "gpio.h"

void gpio_init(void) {
}

static unsigned int *FSEL0 = (unsigned int *)0x20200000; //GPIO 0-9
static unsigned int *SET0 = (unsigned int *)0x2020001c; //GPIO Set 0
static unsigned int *CLR0 = (unsigned int *)0x20200028; //GPIO CLR 0
static unsigned int *LEV0 = (unsigned int *)0x20200034; //GPIO LEV 0

#define DELAY 0x3f0000

void gpio_set_function(unsigned int pin, unsigned int function) {
	check for invalid requests
	if (pin < 0 || pin > 54) {
		return;
	}

	unsigned int* currentFSEL;
	currentFSEL = FSEL0 + (pin/10); // sets FSEL0 to the correct FSELs for GPIO[pin]
	
	// configure GPIO[pin] for output
	unsigned int gpioSectionNumber = (pin%10)*3;
	unsigned int tempNumber = *currentFSEL & ~(7<<gpioSectionNumber);
	*currentFSEL = tempNumber | (function<<gpioSectionNumber);
}

// returns function associated with the pin.
unsigned int gpio_get_function(unsigned int pin) {
	// check for invalid requests
	if (pin < 0 || pin > 54) {
		return GPIO_INVALID_REQUEST;
	}

	// get function from pin
	unsigned int* functionValue;
	functionValue = FSEL0 + pin/10; 

	// unmask function to get only pin section function
	unsigned int gpioSectionNumber = (pin%10)*3;
	unsigned int shiftedFunction = *functionValue>>gpioSectionNumber; // shift to least significant bit
	unsigned int pinFunctionValue = shiftedFunction & 7; //mask to only preserve pin function
    return pinFunctionValue; 
}

void gpio_set_input(unsigned int pin) {
    gpio_set_function (pin, GPIO_FUNC_INPUT);
}

void gpio_set_output(unsigned int pin) {
    gpio_set_function (pin, GPIO_FUNC_OUTPUT);
}

void gpio_write(unsigned int pin, unsigned int value) {
	// check for invalid requests
	if (pin < 0 || pin > 54) {
		return GPIO_INVALID_REQUEST;
	}

	if (value == 1){
		unsigned int* setRegister = (SET0 + (pin/32));
		*setRegister = 1 << (pin % 32); // set GPIO[pin]
	} 
	else {
		unsigned int* clearRegister = (CLR0 + (pin/32));
		*clearRegister = 1 << (pin % 32); // set GPIO[pin]
	}
}

unsigned int gpio_read(unsigned int pin) {
	if (pin < 0 || pin > 54) {
		return GPIO_INVALID_REQUEST;
	}
	// get function from pin
	unsigned int* levRegister = (LEV0 + (pin/32));
	unsigned int levValue = *levRegister;
	levValue = levValue >> (pin % 32);
	levValue = levValue & 1;
	return levValue;
}
