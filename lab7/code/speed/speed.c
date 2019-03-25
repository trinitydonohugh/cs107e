/* File main.c
 * -----------
 * Simple main program with timing code to measure
 * performance of different implementations of strcpy.
 */

#include "assert.h"
#include "strings.h"
#include "timer.h"
#include "printf.h"

// -------------------------------------------------------------------
// Time Trial Helpers
// -------------------------------------------------------------------

// Our time trials will be using strings of this length.
#define COPYLEN (1 << 12)

unsigned _time_trial(char *(*cpy)(char *dst, const char *src))
{
    char src[COPYLEN], dst[COPYLEN];

    for (int i = 0; i < COPYLEN; i++) {
        src[i] = 'X';   // populate src with X's
        dst[i] = (char) i + 1; // populate dest with some non-X values
    }
    src[COPYLEN-1] = '\0';

    unsigned start = timer_get_ticks();
    cpy(dst, src);
    unsigned ticks = timer_get_ticks() - start;

    assert(strcmp(src, dst) == 0);  // verify correct operation
    return ticks;
}

#define TIME_TRIAL(fn) do {           \
    printf("Running " #fn "... ");    \
    printf("took %d ticks\n", _time_trial(fn)); \
} while (0)


// -------------------------------------------------------------------
// Baseline strcpy0, correct implementation but pokey
// -------------------------------------------------------------------

char *strcpy0(char *dst, const char *src)
{
    for (int i = 0; i <= strlen(src); i++) {
        dst[i] = src[i];
    }
    return dst;
}

// -------------------------------------------------------------------
// Wicked fast strcpy functions of your own go here:
// -------------------------------------------------------------------

char *strcpy1(char *dst, const char *src)
{
    return dst;
}




void main(void) {
    timer_init();
    printf("\nStarting time trials now.\n");

    TIME_TRIAL(strcpy0);

    // TODO: Add more TIME_TRIAL calls here for your improved versions
    // of strcpy: TIME_TRIAL(strcpy1), TIME_TRIAL(strcpy2), ...

    printf("\nAll done with time trials.\04");
}
