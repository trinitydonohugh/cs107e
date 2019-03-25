#include "console.h"
#include "printf.h"
#include <stdarg.h>
#include "string.h"
#include "gl.h"
#include "fb.h"
#include "malloc.h"
#include "printf.h"

unsigned int width = 0;
unsigned int height = 0;
unsigned int arrayPointer = 0;
unsigned int textBufSize = 0;

unsigned char *textBuf;

void console_init(unsigned int nrows, unsigned int ncols)
{
	width = ncols;
	height = nrows;
	textBufSize = nrows * ncols;
	textBuf = (char *) malloc(textBufSize); //allocate memory space
	memset(textBuf, ' ', textBufSize); //set mem space to empty

	gl_init(ncols * gl_get_char_width(), nrows * gl_get_char_height(), GL_DOUBLEBUFFER);
}

void console_clear(void)
{
    gl_clear(GL_BLACK); //clear screen
    gl_swap_buffer();
    gl_clear(GL_BLACK); 
    memset(textBuf, ' ', textBufSize); //clear location in memory
   	arrayPointer = 0;
}

void scroll(void) {
	// reset array pointer
	arrayPointer = width * (height - 1); //height stays the same; Width goes back to start
	memcpy(textBuf, textBuf + width, arrayPointer); // shift lines up
	memset(textBuf + arrayPointer, ' ', width); // clear every char in last row
}

int console_printf(const char *format, ...)
{
	// read ... text
	unsigned char buf[textBufSize];
    va_list args; 
    va_start(args, format);
    int bufSize = vsnprintf(buf, textBufSize, format, args);

    //loop through each character
    for (int i = 0; i < bufSize; i++) {
    	unsigned char ch = buf[i];

    	// Scrolling
		if (arrayPointer >= textBufSize) {
			scroll();
		}

		//check for special characters
		if (ch == '\f') {
			console_clear();
		} 
		else if (ch == '\n') {
			arrayPointer = (arrayPointer/ width+1)*width;
		} 
		else if (ch == '\b') {
			if (arrayPointer%width != 0) {
				arrayPointer = arrayPointer - (arrayPointer%width);
			}
		} 
		else if (ch == '\r') {
			arrayPointer = arrayPointer - (arrayPointer%width);
		}

    	textBuf[arrayPointer] = buf[i];
    	arrayPointer++;
    }

    gl_clear(GL_BLACK);
    for (int i = 0; i < arrayPointer; i++) {
    	gl_draw_char((i%width) * gl_get_char_width(), (i/width) * gl_get_char_height(), textBuf[i], GL_WHITE);
    } 

    gl_swap_buffer();

	return bufSize;
}
