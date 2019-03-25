---
released: true
permalink: /labs/lab7/
title: 'Lab 7: Raspberry Pi, Interrupted'
toc: true
readings: |
  [Prelab](/labs/lab7/#prelab-preparation) to prep before lab.
---
{% comment %}
Task list to copy/paste when creating PR for this lab:

__Before releasing lab7:__
- [ ] Review writeup/code/checkin questions (instructor)
- [ ] Walk through (SL)
- [ ] Followup on any github issue from previous

__To prep for lab7:__
- [ ] 

{% endcomment %}

*Lab written by Philip Levis, updated by Pat Hanrahan and Julie Zelenski*

## Learning goals

During this lab you will:

1.  Review the support code for interrupts on the Pi.
2.  Enable GPIO event interrupts for button presses.
2.  Optimize the function `strcpy`. (And perhaps feel the adrenaline rush!)

The main goal of the
lab is to gain experience with configuring and processing interrupts in preparation for your final assignment where you will add interrupts to your keyboard console.


## Prelab preparation
To prepare for lab, do the following:

- Pull the latest version of the `cs107e.github.io` repository.
- Clone the lab repository `https://github.com/cs107e/lab7`.

You will be using your Pi, a breadboard, and two buttons in this lab.

## Lab exercises

Pull up the [check in questions](checkin) so you have it open as you go.

### Interrupts

#### 1) Review interrupts code (30 min)

Change to directory `code/interrupts`.
Gather your partner and review the base code used to support interrupts. This code should be familiar from lecture.

The program start sequence is in the files `start.s` and `cstart.c`. The interrupts module of libpi has the functions to configure and enable interrupts. You can view its source here:  [interrupts_asm.s](https://github.com/cs107e/cs107e.github.io/blob/master/cs107e/src/interrupts_asm.s) and [interrupts.c](https://github.com/cs107e/cs107e.github.io/blob/master/cs107e/src/interrupts.c).  Challenge each other to understand each and every line. Please ask questions if there are things you still find confusing.
Afterwards, answer the questions below.

+  What assembly function (in `start.s`) does your Raspberry Pi boot into and
    what does it do?

+  What function does this function call, and what does that function do?

+  How does the starter code know how long the interrupt table is and safely
    copy it (and nothing more, nothing less) to 0x0?

+  How is a function "attached" as an interrupt handler? If there are multiple interrupt handlers, how is decided which one handles a given interrupt?

+  What piece of state needs to change to globally enable/disable interrupts?



#### 2) Set up a button circuit (15 min)

Set up a simple, one-button circuit on your breadboard. Connect one side of the button to ground and the other side of the button to GPIO pin 25. 

<img title="Button circuit" src="images/button-circuit.jpg" width="400">``

Configure the button circuit so that the default state of the pin is high (1) and pressing the button brings it low (0). The way to do this is to make the pin have a "pull-up resistor".  (We won't use a *physical* resistor; instead, we'll tell the Pi to use an internal resistor.)   In the open circuit when button is not pressed, the resistor "pulls up" the value to 1.  When the button is pressed, it closes the circuit and connects the pin to ground. The value then reads as 0. This is like the behavior of the PS/2 keyboard clock line.  


Next, implement the `button_init` function in `button.c`. 
It should initialize GPIO pin 25 as an input and turn on the pin's pull-up resistor. You may want to look at the similar code you used previously in `keyboard_init` to configure the gpio pins for your keyboard driver.

Implement the `wait_for_click` function to record a button click. The function should:

+ Wait for a falling edge that indicates the button was pressed.
That is, wait until the pin goes from 1 to 0 
(checking its state with `gpio_read`).

+ Use `printf("+")` to report the press.

+ Then wait until the pin is 1 again (that is, wait until you've
released the button).


Compile and run the program to verify that your
`wait_for_click` works correctly.  You'll likely get more than one "click" reported per physical button press. Why do you think this might be? 

When
you're done, double check your understanding with these questions:

1. Name two disadvantages, and one advantage, that the spin-loop implementation
   of detecting clicks has over an interrupt-based
   implementation.

2. Does your program detect exactly one click per physical button
   press? If not, why might this be? Do your observations change if you press and momentarily hold the button down versus a quick press and release?

#### 3) Write an interrupt handler (30 min)

You are now going to rework the program to count button presses via interrupts. Edit `main` to remove the call to `wait_for_clock`, but retain the initialization sequence.

There are several parts to configuring and enabling interrupts. We demonstrate the necessary steps in the function `setup_interrupts`. Review our code and talk over with your partner. Ensure you understand what each call does and why it's necessary. Add a call to `setup_interrupts` in `main` as part of the initialization sequence.
 
Declare a global variable `static int cnt` in `button.c`. This variable tracks the number of presses detected. Your interrupt
handler will increment `cnt` on each press and `main` will watch the value of `cnt` and print whenever it changes.

1. Should `cnt` be declared volatile? Why or why not? Can the compiler tell,
by looking at only this file, how control flows between main and the interrupt handler? Will the compiler generate different code if `volatile` than without it? Will the program behave differently? Test it both ways and find out!

The `button_press` function is attached as an interrupt handler, which sets it up to be called when an interrupt is triggered. The key responsibility of an interrupt handler is to check whether it is intended to process this event and if so, it should handle the event and clear the interrupt. 


Add code to `button_press` to determine if this event is meant for this handler, and if so it should process the event (i.e. increment `cnt`) and clear the event.
Review the [gpioextra.h](https://github.com/cs107e/cs107e.github.io/blob/master/cs107e/include/gpioextra.h) header file to review the available gpio event functions. Which function(s) can be used to check the event status? Which function(s) can be used to clear the event status?

Edit the loop in `main` to read the current value of
`cnt`, compare to the last known value, and print the value whenever it changes.

Compile and run your program. Ensure that it works as expected. That
is, the counter increments on each button press and the
counter is printed when it changes.

Now, edit `button_press` to comment out the step that clears the event. Compile and run the program and see how this changes the program's behavior. What changes and why?

When you're done, discuss and answer the following questions with your 
neighbors.

1. What changes if `cnt` is not declared `volatile`?

1. Describe what is done by each line of code in the `setup_interrupts` function. What would be the effect of removing that line?

1. What happens if the interrupt event is not cleared before returning from the
   handler?

#### 4) Use a ring buffer (10 min)

Review the [ringbuffer.h](https://github.com/cs107e/cs107e.github.io/blob/master/cs107e/include/ringbuffer.h) header file and [ringbuffer.c](https://github.com/cs107e/cs107e.github.io/blob/master/cs107e/src/ringbuffer.c) source. This ADT maintains a simple queue of integer values implemented as a ring buffer.

Create a global variable of type `rb_t *rb` and change `main` to initialize `rb` using `rb_new`. A ring buffer will be used to communicate between the interrupt handler and `main`.
Instead of incrementing a counter in the interrupt handler,
call `rb_enqueue` with the counter's value.
Whereas `main` used to directly read the counter and compare to previous value, instead call `rb_dequeue` to get each update. 

Recompile and ensure that your code works exactly as before.
When you're done, take a moment to verify your understanding:

1. Why might you want to enqueue/dequeue and then return instead of just doing
   arbitrary logic (like drawing on the screen) in the interrupt handler?
2. Why is the significance of the return value from `rb_dequeue`? Why is it essential to pay attention to that return value?

#### 5) Add a second interrupt (15 min)

Add a second button circuit to your breadboard that is a duplicate of the first. Connect the second button to another GPIO pin on your Pi and edit the initialization sequence to configure as an input with pull-up resistor. Configure interrupts to trigger on the falling edge of the second pin.

You can define another handler for this second button or extend the existing handler to handle both buttons. In either case, the handler(s) must determine which pin had the event to determine whether/which to handle.  Maintain
a separate counter and ring buffer for each button. Each time a button is pressed, increment its counter and enqueue the current count on its ring buffer.

Edit your main loop to display each update from either button. How can you determine which ring buffer has received an update?

1. Do the ring buffer variables need to be declared `volatile`? Is it now necessary the the counter variables be declared `volatile`? Why or why not?

### Need for speed (20 min)

As a fun bonus exercise, let's take a look at what you can do when you're done
writing correct code: optimization. With this, your code can start moving
wickedly fast -- seriously.

Change directory to `code/speed` and review the source in the `speed.c`
file.

The program implements `strcpy`, the function to copy a string from
the standard C library, along with a little timer scaffolding to count the
ticks during the function's execution. You can 
refresh yourself on the expected behavior of `strcpy` and related string
library functions by typing in `man strcpy` into your terminal.

The given version works correctly, but is naive to a fault. Build the
code as given, install it to the Pi, and run it to get a baseline tick
count. *It is possible to gain more than a 1000x speedup over the
starter version!*

Take a stepwise approach, so you can measure the effect of a given
modification in isolation:

1. **Copy the starter function into a new version and make a single change.** For example, copy `strcpy0` and rename it `strcpy1` before proceeding.

2. Make a rough prediction about the expected performance gain from that change.

3. Now run and time the new version to see whether your intuition matches
   the observed reality. Where the results surprise you, examine the
   generated assembly to see if you can figure out why there is a different
   effect than you expected.

Repeat
this process, each time advancing from the best version so far and
making another small change.

You may assume that the string buffers our `strcpy`
functions will process are multiples of 16 bytes in size and are
16-byte aligned.

Below are some suggested avenues to explore:

+ A first improvement doesn't even involve changing `speed.c`!
Edit the Makefile to enable various levels of compiler optimization and
rebuild and re-run. What difference do you observe between `-O0`, `-Og`, `-O2` and
`-Ofast`? Pick the best setting and let's see how much you can add to what `gcc` can
do.

+ As the second improvement, that call to `strlen` inside the loop has gotta go!
Hoist it out to earn a shockingly large gain. It's not the function call
overhead that is so high (i.e. inlining `strlen` would not help much if at all),
this is actually a big-O problem: recalculating the string length each
iteration makes the copy function run in quadratic time instead of linear --
ouch! Most optimization focuses on reducing the constant factors for an algorithm
with a given big-O, but if you can restructure into a better big-Oh, that is
generally a much bigger win.

+ Next think about where the function spends time. Recall that every instruction 
contribute a cost: are there ways to change the function so that it does the 
same work with fewer instructions? Take a look at the assembly to see where
the effort is going. In the starter implementation, only a small fraction
of the instructions actually copy the string: the rest are incrementing, 
branching, etc. How could you change the loop to issue fewer of these
overhead instructions?

How big of an improvement were you able to make overall?

## Check in with TA

At the end of the lab period, call over a TA to [check in](checkin) with your progress on the lab.

