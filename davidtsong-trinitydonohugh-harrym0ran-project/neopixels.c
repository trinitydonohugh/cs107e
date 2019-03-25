/* 
This program drives the Flora Neopixels v2


The code is an adapted version of code from the following blog. 
It uses similar logic from the blog, but adapts each function for bare-metal programming on the Raspberry Pi:
https://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/

The repository link from the blog: https://github.com/bigjosh/SimpleNeoPixelDemo/blob/master/SimpleNeopixelDemo/SimpleNeopixelDemo.ino

The LEDs used: Flora RGB Smart Neo Pixel version 2

*/

//___________________________________________________________________________________________________

#include "gpio.h"
#include "printf.h"
#include "strings.h"
#include "stdbool.h"
#include "timer.h"
#include "interrupts.h"
#include "neopixels.h"

volatile unsigned int * const FSEL1 =  (unsigned int *)0x20200004;
volatile unsigned int * const SET0  =  (unsigned int *)0x2020001c;
volatile unsigned int * const CLR0  =  (unsigned int *)0x20200028;

#define LED_PIN GPIO_PIN6  // GPIO LED_PIN 6 on the Raspberry Pi
#define RES 6000    	   // Width of the low gap between bits to cause a frame to latch

#define T1H  700    	   // Width of a 1 bit in ns
#define T1L  600    	   // Width of a 1 bit in ns

#define T0H  300    	   // Width of a 0 bit in ns
#define T0L  800    	   // Width of a 0 bit in ns

static unsigned int NUM_PIXELS; // Number of Neopixels in the string

//Initialize R, G, B values of a color struct with address 'col'
static void neopixels_setColorStruct(color_t *col, unsigned char r, unsigned char g, unsigned char b) {
	(*col).r = r;
	(*col).g = g;
	(*col).b = b;
}

//Initialize all color constants with their respective RGB values, and fill
//the 6 colors in the rainbow[] array.
static void neopixels_initializeColors(void) {
	neopixels_setColorStruct(&RED, 255, 0, 0);
	rainbow[0] = RED;
	neopixels_setColorStruct(&ORANGE, 255, 127, 0);
	rainbow[1] = ORANGE;
	neopixels_setColorStruct(&YELLOW, 255, 255, 0);
	rainbow[2] = YELLOW;
	neopixels_setColorStruct(&GREEN, 0, 255, 0);
	rainbow[3] = GREEN;
	neopixels_setColorStruct(&BLUE, 0, 0, 255);
	rainbow[4] = BLUE;
	neopixels_setColorStruct(&VIOLET, 148, 0, 211);
	rainbow[5] = VIOLET;
	neopixels_setColorStruct(&INDIGO, 75, 0, 130);
	neopixels_setColorStruct(&BLACK, 0, 0, 0);
	neopixels_setColorStruct(&WHITE, 255, 255, 255);
}

//This function initializes the gpio pin to output, initializes the color struct, and clears the LEDS
void neopixels_init(unsigned int num_pixels) {
	NUM_PIXELS = num_pixels;
	neopixels_initializeColors();
	gpio_set_output(LED_PIN);
	neopixels_clear();
}

// We use the assembly loops to send the bit for the precise amount of time for the chip to
// know if the bit is 1H, 1L, 0H, or 0L. Each iteration of the loop takes 120 ns. 
inline void neopixels_sendBit(bool bitVal){
	if (bitVal == 0){ //0 bit
		*SET0 = (0x1 << LED_PIN);
		__asm__ volatile( // 2 * 120 = 440 --> T0H
			"mov r2, #2\n\t" 
			"1: \n\t"
			"subs r2, r2, #1\n\t"
			"bne 1b"
			::: "r2" //clobbering
		);
		*CLR0 = (0x1 << LED_PIN);
		__asm__ volatile(
			"mov r2, #20\n\t" // 20 * 120 = 4400 --> T0L
			"1: \n\t"
			"subs r2, r2, #1\n\t"
			"bne 1b"
			::: "r2"
		);
	}

	else { //1 bit
		*SET0 = (0x1 << LED_PIN);
		__asm__ volatile( // 50 * 120 = 6000 --> T1H
			"mov r2, #50\n\t"
			"1: \n\t"
			"subs r2, r2, #1\n\t"
			"bne 1b"
			::: "r2"
		);
		*CLR0 = (0x1 << LED_PIN);
		__asm__ volatile( // 20 * 120 = 4400 --> T1L
			"mov r2, #20\n\t"
			"1: \n\t"
			"subs r2, r2, #1\n\t"
			"bne 1b"
			::: "r2"
		);
	}
}

//This function iterates through the 8 bits in the given byte and sends each bit to the chip 
inline void neopixels_sendByte( unsigned char byte) {
    
    for( unsigned char bit = 0 ; bit < 8 ; bit++ ) {
      
      neopixels_sendBit( byte & (1 << 7));      // Neopixel wants bit in highest-to-lowest order
                                                // so send highest bit (bit #7 in an 8-bit byte since they start at 0)
      byte <<= 1;                               // and then shift left so bit 6 moves into 7, 5 moves into 6, etc
      
    }         
} 

//This function called neopixels_sendByte for the 3 colors that make up a pixel color: green, red, then blue. 
//By the end of the function, a 24 bit package will be sent
inline void neopixels_sendPixel( color_t color )  {   
  neopixels_sendByte(color.g);          // Neopixel wants colors in green then red then blue order
  neopixels_sendByte(color.r);
  neopixels_sendByte(color.b);
}


//This function creates the necessary delay time in order to communicate to the chip that it should latch onto
//the recently sent pixels on the LEDs  
void neopixels_show() {
	// Round up since the delay must be _at_least_ this long (too short might not work, too long not a problem)
	timer_delay_us( (RES / 1000UL) + 1);				
}


// This function iterates through the number of pixels in the chain, and sends color to all of them
void neopixels_showColor(color_t color) {
	interrupts_global_disable();
  	for (int i = 0; i < NUM_PIXELS; i++){
  		neopixels_sendPixel( color );
	}
	interrupts_global_enable();
  	neopixels_show();
  
}

//This function clears the leds, by sending the color black
void neopixels_clear(){
	neopixels_showColor(BLACK);
}

// This function blinks the LEDS with the inputted color and given wait time
void neopixels_blink(color_t color, unsigned int wait){
	neopixels_showColor(color); 
	timer_delay_ms(wait);
	neopixels_showColor(BLACK);
	timer_delay_ms(wait);
}

/* Lights up the first 'num' LEDs on each pant leg, leaves the rest black */
void neopixels_lightNum(color_t color, unsigned int num) {
	unsigned int i = 0;
	unsigned int pant_leg = NUM_PIXELS/2;
	interrupts_global_disable(); //disable interrupts while sending neopixels
	//first pant leg
	while(i++ <= num){
		neopixels_sendPixel(color);
	}
	while(i++ <= pant_leg){   
		neopixels_sendPixel(BLACK);        		
	}
	//second pant leg
	i = 0;
	while(i++ <= num){
		neopixels_sendPixel(color);
	}
	while(i++ <= pant_leg){   
		neopixels_sendPixel(BLACK);        		
	}
    interrupts_global_enable();
	neopixels_show();
}

/* Show each color of the rainbow, with a 'wait' ms gap between colors. */
void neopixels_rainbow(unsigned int wait) {
	for (int i = 0; i < 7; i++) {
		neopixels_showColor(rainbow[i]);
		timer_delay_ms(wait);
	}
}

/* Generates a random LED pattern for each cycle, in which each pixel
 * has an 80% chance of being on, and its color in chosen randomly 
 * from the rainbow[] array. */
void neopixels_random(unsigned int cycles, unsigned int wait) {
	for (int i = 0; i < cycles; i++) {
		interrupts_global_disable();
		for (int j = 0; j < NUM_PIXELS; j++) {
			unsigned int on = (unsigned int)rand() % 5;
			//roughly 80% of the lights will go on
			if (on <= 3) {
				//choose random index in rainbow array for color
				unsigned int ind = (unsigned int)rand() % 7;
				neopixels_sendPixel(rainbow[ind]);
			} else  
				neopixels_sendPixel(BLACK);
		}
		interrupts_global_enable();
		neopixels_show();
		timer_delay_ms(wait);
		//clear pattern before next cycle
		neopixels_showColor(BLACK);
	}
}


/* 	The following function was adapted from Josh's version of the AdaFruit strandtest code:
	https://github.com/adafruit/Adafruit_NeoPixel/blob/master/examples/strandtest/strandtest.ino
*/
void neopixels_colorWipe(color_t color, unsigned int wait ) {
  for(unsigned int i = 0; i < NUM_PIXELS; i ++){
    
    unsigned int p=0;
    interrupts_global_disable();
    while (p++<=i) {
        neopixels_sendPixel(color);
    } 
    while (p++<=NUM_PIXELS) {
        neopixels_sendPixel(BLACK);        
    }
    interrupts_global_enable();
    neopixels_show();
    timer_delay(wait);
  }
}

//This function drives a simple scanning pattern on the LEDs
void neopixels_scan(color_t color, unsigned int wait){
	for(unsigned int i = 0; i < NUM_PIXELS; i ++){
	    
	    unsigned int p=0;
	    
	    interrupts_global_disable();
	    while (p++<=i) {
	        neopixels_sendPixel(color);
	    } 
	    while (p++<=NUM_PIXELS) {
	        neopixels_sendPixel(BLACK);        
	    }
	    interrupts_global_enable();
	    neopixels_show();
	    timer_delay(wait);
 	}
 	for(unsigned int i = 0; i < NUM_PIXELS; i ++){
	    
	    unsigned int p=0;
	    
	    interrupts_global_disable();
	    while (p++<=i) {	        
	    	neopixels_sendPixel(BLACK);   
	    } 
	    while (p++<=NUM_PIXELS) {
	        neopixels_sendPixel(color);       
	    }
	    interrupts_global_enable();

	    neopixels_show();
	    timer_delay(wait);
 	}
}

//This function was used to calculate how long it took for the assembly loop to finish
//We also did a time test of showColor() and found that it takes 371 ticks to show 8 pixels.
void neopixels_test(){
	int initial_ticks = timer_get_ticks();

	__asm__ volatile(
		"mov r2, #1024\n\t"
		"loop: \n\t"
		"subs r2, r2, #1\n\t"
		"bne loop"
		::: "r2"
	);

	int after_tickes = timer_get_ticks();
	printf("2048 instructions time: %d\n", after_tickes - initial_ticks);
}

