---
released: true
permalink: /labs/lab6/
title: 'Lab 6: Drawing into the Framebuffer'
toc: true
readings: |
  [Prelab](/labs/lab6/#prelab-preparation) to prep before lab.
---
{% comment %}
Task list to copy/paste when creating PR for this lab:

__Before releasing lab6:__
- [ ] Review writeup/code/checkin questions (instructor)
- [ ] Walk through (SL)
- [ ] Followup on any github issue from previous

__To prep for lab6:__
- [ ] Confirm there are sufficient number of working monitors, HDMI cables, and HDMI-DVI adapters (aim for 1 setup for every 2-3 students)

{% endcomment %}

*Lab written by Philip Levis and Pat Hanrahan*

## Goals

In the next assignment,
you will complete the framebuffer module, 
build a basic graphics library,
and create a text console.
Add your keyboard driver from last week, and you will now be able to run your shell 
standalone on the Pi, without your laptop.

The goal of this lab is to review concepts and code
that you will need to complete assignment 6.
During this lab you will:

1. Connect your Raspberry Pi to a monitor and generate video
2. Read and understand the mailbox and framebuffer code
3. Review C syntax for pointers to multi-dimensional arrays
4. Read and understand fonts and the representation of characters

## Prelab preparation
To prepare for lab, do the following: 

- Pull the latest version of the `cs107e.github.io` repository.
- Clone the lab repository `https://github.com/cs107e/lab6`.

## Lab exercises

### 1. Connect your Raspberry Pi to a monitor (5 min)

The first step is to connect your Raspberry Pi to a monitor.
To do this, you need an HDMI cable and an HDMI-to-DVI adapter. (The monitors we are using are older and do not have HDMI connectors).
Attach the HDMI-to-DVI adapter to the monitor's DVI port and
connect the HDMI cable to the adapter and the HDMI port on the Raspberry Pi.
Power your Raspberry Pi, and you should see the following.

![Raspberry Pi video](images/pifb.png)

Now test the GPU on the Raspberry Pi.
Go into the directory `code/grid`.

     $ make

Reset your Pi, and then type

     $ make install

The monitor should now display a grid pattern.

### 2. Draw pixels on your screen (40 min)

Modify the `grid.c` file in the following three ways:

1. Change the video resolution to 1024 by 768, and redraw the grid.

2. Change the grid code to draw horizontal lines in red and vertical lines in yellow.
  *Remember from lecture that the B (blue) in BGRA is the lowest byte.*

3. Change the grid code to draw a checkerboard pattern 
   (alternating filled black and white squares).

### 3. Study the fb and mailbox code (30 min)

#### The GPU mailbox

Recall from [lecture](/lectures/Framebuffer/slides.pdf) that the
CPU and GPU communicate by sending messages through a *mailbox*.
When the sender places a message in the mailbox, it is considered *full*.
When the message is read by the receiver, it becomes *empty*. What is actually read/written to the mailbox is a pointer to the message data.

Overall, the communication pattern we'll be using is:
* The CPU starts the exchange by creating a message 
  and putting a pointer to that message in the mailbox. The mailbox is now full.
* The hardware alerts the GPU to read the message, which empties the mailbox.
* After processing the message,
  the GPU responds by putting a return message in the mailbox 
  (filling it again) for the CPU to read.
* Meanwhile, the CPU is waiting at the mailbox for the GPU's response.
  When the mailbox status changes to full, the CPU reads the return message from the GPU.

#### The configuration message

To configure the framebuffer,
the CPU prepares a _framebuffer configuration message_  and puts it into the mailbox. The CPU's message specifies the desired characteristics for the framebuffer. The reply from the GPU confirms the configured parameters.

In its message, the CPU sets the width, height, and pixel depth of the framebuffer. There are two different sizes to configure: the physical width/height and the virtual width/height.
The physical size describes the size and shape of the screen.
The virtual size describes the size and shape of the pixel data used for the framebuffer. The virtual size can differ from the physical size, and if so, the virtual image will be scaled when displayed on the physical screen.
The fields `size`, `pitch`, and `framebuffer` must be set to 0
in the configuration message.  The proper values for those fields will be returned by the GPU
in its reply message.


The GPU's reply message includes fields for
a pointer to the start of the framebuffer,
the total size in bytes allocated to the framebuffer,
and the length of each scanline or row of the framebuffer
(this is called *pitch*).
The pitch is greater than or equal to
`width` multiplied by the size of a pixel in bytes.
It can be greater than so that the GPU can add some padding at
the end of each row in order to align them.
The pitch is equal to the size of the row plus the padding.
The CPU must set these fields (`size`, `pitch`, and `framebuffer`) to 0
when it sends the configuration message to the GPU:
the GPU responds with the correct values.

Your code should check that the framebuffer
was configured properly by checking that the GPU's reply message has

the same physical and virtual sizes that you sent.
Otherwise, you might have specified a size that the GPU doesn't support,
and your graphics won't work properly.

If the framebuffer is configured correctly, the GPU's reply message
will include a pointer to the image comprising the *framebuffer*.
This is a contiguous block of memory 
that you can access to read and write pixels.
The GPU will continuously display the contents of the framebuffer.
If you change the framebuffer,
the display will be updated automatically.

The other fields in the GPU's reply message are the total size in bytes allocated to the framebuffer and the length of each scanline or row of the framebuffer
(this is called *pitch*).
The pitch is at least equal to the width multiplied by the pixel depth in bytes and will be larger when the GPU has added padding at
the end of each row in order to align them. The `pitch` can be used to divide the framebuffer data into rows.


#### Code to configure the framebuffer

Change into the directory `code/fb`. That directory contains the files:

    $ ls
    Makefile	fb-main.c	fb.h		mailbox.h	start.s
    cstart.c	fb.c		mailbox.c	memmap

The `fb.c` contains the function `fb_init()` that was shown in class.
This function configures the `fb_config_t` structure 
(the framebuffer configuration message),
and sends it to the GPU using `mailbox_write` and `mailbox_read`.
These mailbox functions are defined in `mailbox.c`.

Read and discuss the code in `fb.c` , `mailbox.h`, and `mailbox.c`
with your lab neighbors. Discuss the questions below as a group and ask the TA to clarify if you have any confusion.

   1. Why does the code need each of the checks for whether the mailbox is `EMPTY`
      or `FULL`? What might go wrong if these checks weren't there?

   2. Why can we add the `addr` and `channel` in `mailbox_write`?
      Could we also `|` them together?
      Which bit positions are used for the `addr` and which are used for the `channel`?

   3.  Sketch a memory map diagram of where `fb`, `mailbox`, and the framebuffer live.
      Clearly mark where the CPU's memory and GPU's memory are, as well as
      non-RAM device registers.  Recall that your Pi is configured to give the 
      bottom 256MB of memory (0x00000000 - 0x0ffffffff) to the CPU and the top 256MB 
      (0x10000000 - 0x1fffffff) to the GPU.  Do `fb`, `mailbox`, and the framebuffer live 
      in GPU or CPU memory?   Which of these
      data structures can we choose where to allocate, and which are given to us?

   4. Change to the directory `code/volatile`. This contains a slightly
      modified version of the code from the `fb` directory. The Makefile
      compiles two versions of the mailbox code; one version marks the mailbox
      as `volatile`, the other that does not.
      Run `make` and compare the assembly in the file `mailbox.list` to `mailbox-not-volatile.list`.
      How does the assembly differ? What happens to the first loop branch in the read or write operations? Why?

### 4. Multidimensional pointers (30 min)

Pointers are one of the hardest concepts in C.
The goal of this part of the lab is to review 
pointers to multidimensional arrays
to better prepare you for the assignment.

One convenient way to represent
images is with a two-dimensional array.
Treating it as a 2D array
can be much easier
than explicitly calculating offsets into a one-dimensional array.

To start, here is a quick self-test:

* What is the difference between the following two declarations?
  Think about what operations are valid on `a` versus `b`.

      char *a  = "Hello, world\n";
      char b[] = "Hello, world\n";

* What is the difference between the following two declarations?

      int *p[2];
      int (*c)[2];

  You may find **[this website](http://cdecl.org/)** helpful in demystifying
  the meaning of various C declarations.

Inspect the code in `code/pointers/pointers.c`. Compile the program using `make`, run it on your Pi, and interpret the results.


### 5. Fonts (15 min)

A major part of your assignment will be to draw text on the screen.
In order to do this, you need a *font*.

![Font](images/Apple2e.bmp)

This is a very famous font: do you recognize what computer it came from? Our library font module uses this font to provide that extra-special retro touch for your graphical console.

Take a look at the files [font.h](https://github.com/cs107e/cs107e.github.io/blob/master/cs107e/include/font.h) and [font.c](https://github.com/cs107e/cs107e.github.io/blob/master/cs107e/src/font.c). The file `font.h` declares a `font_t` struct for representing a font and the `font.c` defines a variable of `font_t` type.

```
/* from font.h */
typedef struct  {
    unsigned char first_char, last_char;
    unsigned int  char_width, char_height;
    unsigned char pixel_data[];
} font_t;
```

```
/* from font.c */
static const font_t font_default = {
    0x21, 0x7F, 14, 16, {
    0x0c, 0x00, 0xcc, 0x03, 0x30, 0x03, 0x00, 0xf0, 
    0x00, 0xc0, 0x00, 0xc0, 0x03, 0x00, 0x0c, 0x00, 
     ...
    0xfc, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00 }
};
```

The character data for the font is stored as a bitmap. 
In a bitmap, each pixel is either on or off and is represented by a single bit.
'On' means the pixel is drawn in the foreground color,
'off' means the pixel is set to the background color.
We use a bitmap rather than RGBA because it takes much less (32 times less) memory.
This makes a program that uses fonts much smaller,
and hence faster to upload to your Raspberry Pi.

Below is an image bmp file generated from `font_default` using green to display each 'on' pixel in the bitmap. (click the image to see larger version):

[<img title="Font" src="images/apple2e-line.bmp">](images/apple2e-line.bmp)

The characters are stored in a single line.
The leftmost character is '`!`', whose ASCII value is 33 (0x21).
The rightmost character is ASCII value 127 (0x7e) Delete, which is displayed as a little checkerboard.
The font contains 95 characters in total.

Each character is the same size: 14 pixels wide and 16 pixels tall.
This is termed a *fixed-width* font.

Each line of the image is 1330 pixels long (95 characters * 14 pixels wide),
and requires 1330 bits to store in bitmap form.
The bitmap is represented 
as an array of `unsigned char` with values assigned in hexadecimal.
For example, the first two bytes in the array are `0x0c, 0x00`.
Group the 8 bits from the first byte and 6 bits from the second to form the 14-bit sequence `0b 00001100 000000`.  These 14 bits correspond to the top row of the first character in the font, which is an exclamation point. The vertical line for the exclamation point is 2 pixels wide and positioned slightly off-center to the left.


* Talk with your neighbor: why does pixel_data have size `95 * 14 * 16 / 8`?

Look carefully at the function `font_get_char()` in `font.c` which copies a single character
from the font image into a buffer. It transforms the bits of the image into an array of bytes, one byte per pixel. Read this function carefully,
since you will use it in the next assignment.

## Check in with TA

At the end of the lab period, call over a TA to [check in](checkin) with your progress on the lab.

If you haven't made it through the whole lab, we still highly encourage you to
go through the parts you skipped over, so you are well prepared to tackle the assignment.
