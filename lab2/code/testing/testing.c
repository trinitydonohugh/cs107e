// File: testing.c
// ---------------
// Unit testing example

#include "assert.h"

// The is_odd function is intended to return a true/false value
// that reports whether its argument is odd. But oops-- the code as 
// written below is buggy!  Something this simple we could validate
// "by inspection" but we are going to use this as an opportunity
// to show the use of rudimentary unit tests. The assert() function
// can be used to validate whether an expression returns the
// expected result. You assert a condition that must be true
// and if the expression evaluates to false, an error is raised.
// assert() uses the LEDs on board the Pi as a status indicator. 
// When an assert fails, it stops executing the program
// and flashes the red LED. If the program finishes executing and 
// no test was failed, the green LED turns on.
// Thus if you get the flashing red LED of doom, you know your
// program is failing at least one test.
// The solid green light means your program passed the tests you
// asked it to check.

int is_odd(int n)
{
    return (n & 0x1) != 0;  // OOPS buggy!
}

int is_even(int n)
{
    return !is_odd(n);  // would be fine, if is_odd worked
}

void main(void) {
    assert(is_odd(3));
    assert(is_even(0));
    assert(is_odd(107));
    assert(is_odd(5));
    assert(is_even(4));
    assert(is_even(10));
    assert(is_odd(15));
    return;
	// read cstart.c to find out what happens after main() finishes
}
