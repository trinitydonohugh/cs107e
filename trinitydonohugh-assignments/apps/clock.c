#include "../gpio.h"
#include "../timer.h"

//array with all patturn binary representation
int listOfBitPatturns[16] = 
{
	0b00111111, //0x7e //display 0
	0b00110000, //0x06 //display 1
	0b01011011, //0x6d //display 2
	0b01001111, //0x79 //display 3
	0b01100110, //0x33 //display 4
	0b01101101, //0x5b //display 5
	0b01111101, //0x5f //display 6
	0b00000111, //0x70 //display 7
	0b01111111, //0x7f //display 8
	0b01101111, //0x7b //display 9
 	0b01110111, //0x77 //display A
 	0b01111111, //0x7f //display B
 	0b00111001, //0x4e //display C
 	0b00111111, //0x7e //display D
 	0b01111001, //0x4f //display E
 	0b01110001 //0x47 //display F
};

//function for diaplying patturn on each digit
void displayPatturn (int arrayValue) {

	unsigned int gpioNumber = GPIO_PIN20;

    //turn patturn display pins on
    unsigned int outputValue = listOfBitPatturns[arrayValue];
    for (int i = 0; i < 8; i++) {
    	if (outputValue%2 == 1) {
    		gpio_write(gpioNumber, 1);
    	}
    	outputValue = outputValue/2;
    	gpioNumber++;
    }
    
    //delay timer to display patturn 
    timer_delay_us(2500);

    //turn patturn display pins off
    gpioNumber = GPIO_PIN20;
    outputValue = listOfBitPatturns[arrayValue];
    for (int i = 0; i < 8; i++) {
    	if (outputValue%2 == 1) {
    		gpio_write(gpioNumber, 0);
    	}
    	outputValue = outputValue/2;
    	gpioNumber++;
    } 
}

//function for changing all digits and patturns on the clock
void changeScreenValue (unsigned int digitToChange, unsigned int valueToChange) {
	gpio_write(digitToChange, 1);
	displayPatturn(valueToChange);
	gpio_write(digitToChange, 0);
}

void main(void)
{
	//set gpio pins for digits
    for (int i = 10; i < 14; i++) {
    	gpio_set_output(i);
    }

    //set gpio pins for patturns 
    for (int i = 20; i < 28; i++) {
    	gpio_set_output(i);
    }

    //get time from timer 
    volatile unsigned int startTime = timer_get_ticks();
    unsigned int seconds = 0;
    unsigned int minutes = 0;
    unsigned int tempSeconds = seconds;
    timer_delay(1);
    unsigned int ticksPerSecond = timer_get_ticks() - startTime;

    //loop for displaying clock changes
    while (1) {
    	if (timer_get_ticks() - startTime > ticksPerSecond) {
    		seconds++;
    		tempSeconds++;
    		minutes = seconds/60;
    		startTime = timer_get_ticks();
    	}
    	if (tempSeconds == 60) {
    	 	tempSeconds = 0;
    	}
    	changeScreenValue(10, (tempSeconds%10));
    	changeScreenValue(11, (tempSeconds/10));
    	changeScreenValue(12, (minutes%10));
    	changeScreenValue(13, (minutes/10));
    }
}
