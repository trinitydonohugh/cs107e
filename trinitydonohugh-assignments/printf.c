#include "printf.h"
#include <stdarg.h>
#include "string.h"

#define MAX_OUTPUT_LEN 1024

// returns the length of the value passed into unsigned_to_base 
int findLengthVal (unsigned int val, int base) {
    unsigned int lengthOfVal = 0;
    while (val != 0) {
        val = val/base;
        lengthOfVal++;
    }
    return lengthOfVal;
}

int unsigned_to_base(char *buf, size_t bufsize, unsigned int val, int base, int min_width)
{
    // declare tempVal to hold string 
    char tempBuf[findLengthVal(val, base) + min_width];
    int tempVal = val;
    int tempBufLength = 0;

    // deals with case where the value passed in is 0. 
    if (tempVal == 0) {
        if (bufsize >= 2) {
            buf[0] = '0';
            buf[1] = '\0';
        } else if (bufsize == 1) {
            buf [0] = '\0';
        }
        return 1;
    }

    // adds val passed into string tempBuf in reverse order
    while (tempVal != 0) {
        if (tempVal%base >= 0 && tempVal%base <= 9) {
            tempBuf[tempBufLength] = (char)((tempVal%base)+'0');
        }
        else if (tempVal%base >= 10 && tempVal%base <= 15) {
            tempBuf[tempBufLength] = (char)((tempVal%base)+'a'-10);
        }
        tempVal = tempVal/base;
        tempBufLength++;
    }

    // adds preceeding zeros to the end of tempBuf to ensure string fits min_width
    while ((min_width-(tempBufLength)) > 0) {
        tempBuf[tempBufLength] = '0';
        tempBufLength++;
    }

    //reverse string tempBuf to be in correct order
    int bufLengthCounter = 0;
    if (bufsize > tempBufLength) {
        while (tempBufLength > 0) {
            buf[bufLengthCounter] = tempBuf[tempBufLength-1];
            bufLengthCounter++;
            tempBufLength--;
        }
        buf[bufLengthCounter] = '\0';
        return bufLengthCounter;
    } 
    else {
        while (bufsize > 0) {
            buf[bufLengthCounter] = tempBuf[tempBufLength-1];
            bufLengthCounter++;
            tempBufLength--;
            bufsize--;
        }
        buf[bufLengthCounter] = '\0';
        return bufLengthCounter;
    }
}

int signed_to_base(char *buf, size_t bufsize, int val, int base, int min_width)
{
    // if value is not negative calls unsigned_to_base
    // if value is negative adds a negative to the beginning then followed by string from 
    // unsigned_to_base
    if (val > -1) {
        return unsigned_to_base(buf, bufsize, val, base, min_width);
    } else {
        char *tempBuf = buf + 1;
        int unsignedValue = ~val + 1;
        int lengthOfBuf = unsigned_to_base(tempBuf, bufsize, unsignedValue, base, min_width);
        *buf = '-';
        return lengthOfBuf+1;
    }
}

int vsnprintf(char *buf, size_t bufsize, const char *format, va_list args)
{
    char bufTemp[MAX_OUTPUT_LEN]; //string structure to store display value/string/char
    int bufTempCounter = 0;
    // loop through the formatting code
    for (int i = 0; i < strlen(format); i++) {
        if (format[i] == '%') {
            if (format[i + 1] == 'c') {
                bufTemp[bufTempCounter] = (char)va_arg(args, int); //reads value from args to bufTemp
                bufTempCounter++;
                i++;
            } 
            else if (format[i + 1] == 's') {
                char *bufValue = va_arg(args, int);
                memcpy(&bufTemp[bufTempCounter], bufValue, strlen(bufValue));
                bufTempCounter = bufTempCounter + strlen(bufValue);
                i++;
            }
            else if (format[i + 1] == 'p') {
                void *value = va_arg(args, void *);
                // add 0x preceding address 
                bufTemp[bufTempCounter] = '0';
                bufTempCounter++;
                bufTemp[bufTempCounter] = 'x';
                bufTempCounter++;
                //convert args read to string 
                int length = unsigned_to_base(&bufTemp[bufTempCounter], MAX_OUTPUT_LEN, (unsigned int)value, 16, 0);
                bufTempCounter = bufTempCounter + length;
                i++;
            }
            else if (format[i + 1] == '%') {
                bufTemp[bufTempCounter] = '%';
                bufTempCounter++;
                i++;
            }
            else if (format[i + 1] == '0') { //deals with pad cases
                //get length of % to d/x
                const char *rest;
                int minWidth = strtonum(&format[i+1], &rest);
                i = rest - format; // resets i value for length of pad value
                if (format[i] == 'd') {
                    int value = va_arg(args, int);
                    bufTempCounter += signed_to_base(&bufTemp[bufTempCounter], MAX_OUTPUT_LEN, value, 10, minWidth);
                    i++;
                }
                else if (format[i] == 'x') {
                    char *hexValue = (unsigned int)va_arg(args, unsigned int);
                    bufTempCounter += signed_to_base(&bufTemp[bufTempCounter], MAX_OUTPUT_LEN, hexValue, 16, minWidth);
                    i++;
                }
            } 
            else { // deals with non-pad cases
                if (format[i + 1] == 'd') {
                    int value = va_arg(args, int);
                    bufTempCounter += signed_to_base(&bufTemp[bufTempCounter], MAX_OUTPUT_LEN, value, 10, 0);
                    i++;
                }
                else if (format[i + 1] == 'x') {
                    char *hexValue = (unsigned int)va_arg(args, unsigned int);
                    bufTempCounter += signed_to_base(&bufTemp[bufTempCounter], MAX_OUTPUT_LEN, hexValue, 16, 0);
                    i++;
                }
            }
        }
        else {
            *(bufTemp + bufTempCounter) = format[i];
            bufTempCounter++;
        }
    }
    bufTemp[bufTempCounter+1] = '\0'; // add null pointer to signal end
    memcpy(buf, bufTemp, bufsize); // copy string from tempBuf to buf keeping in mind bufsize
    //return length of bufTemp
    return bufTempCounter;
}


int snprintf(char *buf, size_t bufsize, const char *format, ...)
{
    va_list args; 
    va_start(args, format);
    int returnValue = vsnprintf(buf, bufsize, format, args);
    va_end(args);
    return returnValue;
}

int printf(const char *format, ...)
{
    char buf[MAX_OUTPUT_LEN];
    va_list args; 
    va_start(args, format);
    int bufSize = vsnprintf(buf, MAX_OUTPUT_LEN, format, args);
    for (int i = 0; i < bufSize; i++) {
        uart_putchar(buf[i]);
    }
    va_end(args);
    return bufSize;
}
