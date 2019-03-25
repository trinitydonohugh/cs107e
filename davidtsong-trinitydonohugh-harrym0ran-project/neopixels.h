/*
The LEDs used: Flora RGB Smart Neo Pixel version 2
*/

#ifndef NEOPIXELS_H
#define NEOPIXELS_H

#include "stdbool.h"
#include <stdlib.h>

typedef struct color_t{
    unsigned char r;
    unsigned char g;
    unsigned char b;
} color_t ;

color_t RED;
color_t ORANGE;
color_t YELLOW;
color_t GREEN;
color_t BLUE;
color_t INDIGO;
color_t VIOLET;
color_t BLACK;
color_t WHITE;

color_t rainbow[6];

void neopixels_init(unsigned int num_pixels);

void neopixels_sendBit(bool bitVal);

void neopixels_sendByte( unsigned char byte );

void neopixels_sendPixel( color_t color );

void neopixels_show(void);

void neopixels_showColor(color_t color);

void neopixels_clear();

void neopixels_blink(color_t color, unsigned int wait);

void neopixels_colorWipe(color_t color, unsigned int wait );

void neopixels_rainbow(unsigned int wait);

void neopixels_lightNum(color_t color, unsigned int num);

void neopixels_random(unsigned int cycles, unsigned int wait);

void neopixels_rainbowCycle(unsigned char frames , unsigned int frameAdvance, unsigned int pixelAdvance );

void neopixels_test(void);

#endif
