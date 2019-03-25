#include "strings.h"

void *memset(void *s, int c, size_t n)
{
    unsigned char charC = c;
    char *tempPtr = s;

    for (int i = 0; i < n; i++) {
        *tempPtr = charC;
        tempPtr++;
    }

    return s;
}

void *memcpy(void *dst, const void *src, size_t n)
{
    char *tempDst = dst;
    const char *tempSrc = src;
    for (int i = 0; i < n; i++) {
        *tempDst = *tempSrc;
        tempDst++;
        tempSrc++;
    }
    return tempDst;
}

size_t strlen(const char *s)
{
    /* Implementation a gift to you from lab3 */
    int i;
    for (i = 0; s[i] != '\0'; i++) ;
    return i;
}

int strcmp(const char *s1, const char *s2)
{
    char *tempS1 = s1;
    char *tempS2 = s2;
    for (int i = 0; i < strlen(tempS1); i++) {
        if (tempS1[i] > tempS2[i]) {
            return 1;
        }
        else if (tempS1[i] < tempS2[i]) {
            return -1;
        } 
        else {
            return 0;
        }
    }
}

size_t strlcat(char *dst, const char *src, size_t maxsize)
{
    char *tempDst = dst;
    for (int i = 0; i < strlen(dst); i++) {
        tempDst++;
    }

    char *tempSrc = src;

    while (maxsize - strlen(dst) - 1 != 0 && *tempSrc != '\0') {
        tempDst++;
        *tempDst = *tempSrc;
        tempSrc++;
    }
    *tempDst = '\0';
    return (strlen(dst) + strlen(src));
}

unsigned int strtonum(const char *str, const char **endptr)
{
    char *tempStr = str;
    unsigned int intStr = 0;

    //reads in hex or dec value and coverts it into it's ascii equvilant 
    // and then passes it into a str. 
    if (*tempStr == '0' && *(tempStr+1) == 'x') {
        unsigned int hexFlag = 1;
        tempStr = tempStr + 2;
        while (hexFlag == 1) {
            if ((*tempStr >= '0') && (*tempStr <= '9')) {
                intStr = (intStr*16) + (*tempStr - '0');
                tempStr++;
            }
            else if ((*tempStr >= 'A') && (*tempStr <= 'F')) {
                intStr = (intStr*16) + (*tempStr - 'A' + 10);
                tempStr++;
            }
            else if ((*tempStr >= 'a') && (*tempStr <= 'f')) {
                intStr = (intStr*16) + (*tempStr - 'a' + 10);
                tempStr++;
            }
            else {
                hexFlag = 0;
            }
        }
        *endptr = tempStr;
        return intStr;
    } 
    else if ((*tempStr >= '0') && (*tempStr <= '9')) {
        unsigned int decFlag = 1;
        tempStr++;
        while (decFlag == 1) {
            if ((*tempStr >= '0') && (*tempStr <= '9')) {
                intStr = (intStr*10) + (*tempStr - '0');
                tempStr++;
            }
            else {
                decFlag = 0;
            }
        }
        *endptr = tempStr;
        return intStr;
    } 
    else {
        return 0;
    }
}
