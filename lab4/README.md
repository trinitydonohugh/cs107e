---
released: true
permalink: /labs/lab4/
title: 'Lab 4: Linked and Loaded'
toc: true
readings: |
  [Prelab](/labs/lab4/#prelab-preparation) to prep before lab.
---
{% comment %}
Task list to copy/paste when creating PR for this lab:

__Before releasing lab4:__
- [ ] Review writeup/code/checkin questions (instructor)
- [ ] Walk through (SL)
- [ ] Followup on any github issue from previous

__To prep for lab4:__
- [ ] Make copies of bootloader.c and memory diagram to hand out in lab

{% endcomment %}

*Lab written by Pat Hanrahan, updated by Julie Zelenski*

## Goals

During this lab you will:

1. Experiment with the linker `ld`, with the goal of understanding what is in
   an object (ELF) file by using `nm`, and how object files are combined to produce an
   executable.
2. Understand how executing programs are laid out in memory, including the organization of stack frames.
3. Read and understand the source to the bootloader. It is important to be able
   to read code produced in the wild.
4. Explore the stack and heap in preparation for assignment 4.


## Prelab preparation
To prepare for lab, do the following: 

- Pull the latest version of the `cs107e.github.io` repository.
- Clone the lab repository `https://github.com/cs107e/lab4`. 
- If you didn't get a chance to complete the stack debugging exercise in last week's lab ([Lab3 exercise 1b](/labs/lab3/#1b)), please do it now. This will give you further practice with gdb and introduce you to the commands for tracing function calls and examining stack frames.
- Having a solid understanding of the memory layout of your program is essential for your next assignment. Review the program in [simple.c](code/simple/simple.c). Study the code alongside this [diagram of the program's address space](images/stack_abs.html) when stopped in the first call to `abs`. Having examined this in advance will be helpful when exploring the stack frame layout in exercise 4 below.

## Lab exercises

Pull up the [check in questions](checkin) so you have it open as you go.

### 1. Linking

In the first part of this lab, you will be repeating some of the live coding demonstrations shown during lecture on linking and loading.

Let's first review some terminology. An _object file_ (also called an .o file or a relocatable) is the result of compiling and assembling a single source file. An object file is on its way to becoming a full program, but it's not finished. The linker takes over from there to combine the object file with  additional object files and libraries. The linker is responsible for resolving inter-module references and relocating symbols to their final location. The output of the linker is an _executable_ file, this represents a full program that is ready to run.

#### Symbols in object files

Change to the `code/linking` directory of `lab4`.

The `nm` program allows us to examine the symbols (i.e. function names, variables, constants, etc.) in an object file. Try it out now:

    $ make clean
    $ make start.o
    $ arm-none-eabi-nm -n start.o

The `nm` output lists the symbols from the object file, one symbol per line line.
What symbols are listed for `start.o`?
What is the significance of the numbers shown in the left column?
What do the single letters `T`, `U`, and `t` in the second column mean?
In another window, you may want
to open `start.s` in a text editor for comparison.

Skim the `arm-none-eabi-nm` [man page](https://manned.org/arm-none-eabi-nm) to learn a little bit about this tool. What are some of the other symbol types you expect to see? What is the significance of upper versus lowercase for the symbol type? What does the `-n` flag do?

*Make sure you and your partner understand `nm`'s
output before continuing.*

Let's now examine the symbols for the other object files.

    $ make linking.o
    $ arm-none-eabi-nm -n linking.o

How many symbols are listed for `linking.o`? 
What do the single letter symbols `D`, `R`, `C`, and `b` mean in the `nm` output?
Open `linking.c` in a text editor and try to match each variable with a symbol in the `nm` output.
Can you find them all? Why are there no stack variables in the symbol list?

What type of symbol do you think the declaration `const int[10]` would be? What will be the size of this symbol?
Uncomment line 10 in `linking.c` with `const int n_array[10];` recompile `linking.o` and use `arm-none-eabi-nm -S linking.o` to see for yourself.

Finally, let's see what `arm-none-eabi-nm` tells us about the symbols in
`cstart.o`.

    $ make cstart.o
    $ arm-none-eabi-nm -n cstart.o

#### Symbols in an executable

The final build step is to ask the linker to join the object files and libraries into one combined program. The three object files we just examined are linked together to form the executable `linking.elf`.  Use `make linking.elf` to perform the link step.

    $ make linking.elf
    $ arm-none-eabi-nm -n linking.elf

The executable contains the union of the symbols we saw earlier in the three object files. However there has been some rearrangement. What is the order of the symbols in the executable? How have their addresses changed during the link process?
Do any undefined symbols remain?
What happened to the symbols marked `C`?

#### Resolution and relocation

Let's use our .list files to review the assembly to get a better understanding of how symbols are resolved and relocated.

```
$ make start.list
$ more start.list

00000000 <_start>:
       0:   mov     sp, #134217728  ; 0x8000000
       4:   mov     fp, #0
       8:   bl      0 <_cstart>

0000000c <hang>:
       c:   b       c <hang>
```

The third instruction (at offset 8) is where `_start` calls `_cstart`. The *branch and link* instruction `bl` branches to location 0.
Is 0 the address of `_cstart`?

Now let's look at the listing for `linking.elf`, 
and inspect the same code after linking.

```
$ make linking.elf.list
$ more linking.elf.list
 00008000 <_start>:
     8000:       mov     sp, #134217728  ; 0x8000000
     8004:       mov     fp, #0
     8008:       bl      811c <_cstart>

 0000800c <hang>:
     800c:       b       800c <hang>

 00008010 <sum>:
     8010:       mov     ip, sp
     8014:       push    {fp, ip, lr, pc}
           ...
     8118:       bx      lr

 ...

 0000811c <_cstart>:
     811c:       mov     ip, sp

 ...
```

What is the difference between the branch address in `start.o` 
before and after linking?
What did the linker do to change the address?

#### Libraries

Next, `cd` into `../libmypi`.
This directory contains an example of building a library
`libmypi.a` containing the files `gpio.o` and `timer.o`.

Read the `Makefile`.
Notice the lines

    libmypi.a: $(LIBRARY_MODULES)
    	arm-none-eabi-ar crf $@ $^

The `arm-none-eabi-ar` program creates an *archive* from a list of object files.
The flags `crf` mean to create (`c`) the archive,
replace/insert (`r`) the files,
and use the filename (`f`) for the name of the archive.

The library can then be passed to the linker using `-lmypi`.

The linker treats objects files (`.o`) and libraries (`.a`) a little bit differently.
When linking object files, all the files are combined.
When linking libraries, only files containing definitions of
undefined symbols are added to the executable.
This makes it possible to make libraries with lots of useful modules,
and only link the ones that you actually use in the final executable.

### 2. Memory Map

As part of the relocation process, the linker places all of the symbols into their final location. You must supply a _memory map_ to the linker to indicate the layout of the sections. Let's look into this file to better understand its purpose and function.

Change to the `lab4/code/linking` directory and use `nm` to see the final locations of all the symbols in the executable.

    $ arm-none-eabi-nm -n linking.elf

Note how all symbols of a given type (text, data, rodata, etc.) are grouped together into one section.

Now open the file `memmap` in your text editor. `memmap` is a _linker script_, which tells the linker how to lay out the sections in the final executable file.

- Do you see how the `memmap` linker script specifies how sections from
  individual files are to be combined?

- One of the purposes of `memmap` is to ensure that the global label `_start`
  appears first in the executable file. Why is it critical that this 
  function be first? How does `memmap` specify where `start.o`
  should be placed?

- Another purpose of `memmap` is to setup block storage of uninitialized
  variables. Look for the symbols `__bss_start__` and
  `__bss_end__`. Where are those symbols placed? In a C program, unset global
  variables are to be initialized to 0 at program start. How does `_cstart` use the bss start/end symbols to
  zero the unset variables?

Our provided `memmap` is a valid linker script for a standard bare-metal C program for the Pi. You are unlikely to need to edit or customize it.  However, if you are curious to know more,
here is [documentation on linker scripts](https://sourceware.org/binutils/docs-2.21/ld/Scripts.html).

### 3. Bootloader
The _bootloader_ is the program that runs on the Raspberry Pi that waits to receive a program from your laptop and then executes it. Back in lab 1, you downloaded `bootloader.bin` from the [course firmware](https://github.com/cs107e/cs107e.github.io/tree/master/firmware) folder and copied it to your SD card under the name `kernel.img`. This makes it the program that starts up when the Raspberry Pi resets.

So far, we have used the bootloader as a "black box". Now you are ready to open it up and learn how programs are sent from your laptop and execute on the Pi.

The bootloader we are using is a modified version of one written by 
David Welch, the person most
responsible for figuring out how to write bare metal programs on the Raspberry
Pi. If it wasn't for [his great work](https://github.com/dwelch67/raspberrypi), we would not be offering this course!

Your laptop and the bootloader communicate over the serial line via the Raspberry Pi's UART. They use a simple file transfer protocol called XMODEM. In the jargon of XMODEM, your laptop initiates the transfer and acts as the _transmitter_; the bootloader acts as the _receiver_.

#### Transmit: `rpi-install.py`

The  `rpi-install.py` is a Python script that runs on your laptop and transmits a binary program to the waiting bootloader.

![xmodem image](images/xmodem.jpg)

The transmitter reads in the binary file to transmit and sends 
the file contents chunked into a sequence of 128-byte packets. Here is the algorithm
used in the transmitter:

1. Start the transmission by sending an `SOH` byte, which has value
   `0x01`. `SOH` is a _control character_ which stands for _start of heading_;
   it indicates the start of a new packet.

2. Send the packet sequence number as a byte. The first packet is numbered 1 and 
   the sequence number is incremented for each subsequent packet.

3. Send the bitwise complement of the packet sequence number as a byte.

4. Send the next 128 bytes of data. (one packet's worth)

5. Send the checksum of the packet as a byte. The checksum is computed by summing all bytes in the packet (mod 256).

6. Repeat steps 1-5 for each packet. When there are no 
   more packets to be sent, send an `EOT` byte. `EOT` stands for _end of transmission_ and has value `0x04`.

`rpi-install.py` is the transmitter that runs on your laptop.  This script is written in python (a great language, by the way) and is compatible with any platform that has the proper python support. Given the handy python libraries to abstract away the details of the XMODEM protocol-handling, the script code doesn't show the nitty-gritty of the send/receive mechanics. In fact, the bulk of the script is goop used to identify where to contact the CP2102 driver on different platforms. Read over the script for yourself by [viewing rpi-install.py](https://github.com/cs107e/cs107e.github.io/tree/master/cs107e/bin/rpi-install.py) in our courseware repo or using this command in your terminal:
```
$ less `which rpi-install.py`
```

#### Receive: `bootloader.bin`

The `bootloader.bin` you have installed on your SD card is a C program that runs bare-metal on the Raspberry Pi. Change to the directory `lab4/code/bootloader`. This directory contains the source code to build `bootloader.bin`. This program waits to receives a binary from your laptop which it loads into memory, and then branches to the code to begin execution.

First, read the assembly language file `start.s`.
Note the `.space` directive between `_start` and the label `skip`.
This has the effect of placing the bootloader code
at location `0x200000`.
This creates a hole in memory
(between 0x8000 and 0x200000).
The bootloader loads your program into that hole.
Why can't the bootloader code also be placed at 0x8000?

Now let's look at the algorithm used to receive the XMODEM
protocol, as implemented in the file `bootloader.c`.
This program, which runs on the Raspberry Pi, is the receiver.

It reads bytes by doing the following:

1. **Wait for a SOH (start of heading) byte**

2. **Read the packet number**

   The first block must be packet number 1.
   After a packet is successfully received, the packet number is incremented
   for the subsequent packet.
   If the receiver sees a packet with the wrong or out-of-sequence packet number,
   the receiver sends the control character `NAK`, for
   negative acknowledge, to the transmitter.

3. **Check that the complement of the packet number is correct**

   The receiver verifies that the complement is consistent with the packet number. If mismatched, the receiver sends a `NAK` to the transmitter.

   What C operations are used in the bootloader to validate the complement?

4. **Read payload while updating checksum**

   Read the 128 bytes comprising the payload of the packet. Each payload byte is copied to memory in sequence.
   Update the checksum to include each byte as it arrives.
   The checksum is formed by adding together (mod 256) all the bytes in the packet.

   Look at the code which computes the checksum in the bootloder.
   Suppose the transmitter sends 128 bytes,
   where the 1st byte is 1, the 2nd byte is 2, and so on, until
   we get to 128th byte which has the value 128.
   What is the value of the checksum in this case?

5. **Depending on checksum, send ACK or NAK**

   After 128 bytes have been read,
   read the checksum byte sent by the transmitter.
   Compare that checksum
   with the calculated checksum.
   If they match, send an `ACK` (acknowledge) to the transmitter;
   If mismatched, send a `NAK` (negative acknowledge) to the transmitter.

6. **Check for EOT (end of transition) byte**

   Upon receipt of an `EOT` byte, the receiver knows the transmission is complete.
   Send an `ACK`,
   and then jump to the memory location where the program was loaded

Where does the bootloader copy the program?
What happens when the bootloader detects an error
which requires it to send a `NAK`?
How does it recover from errors?
Why does it use the timer?

Discuss the bootloader code in detail with your labmates.
Make sure you understand all of it.

Each group is responsible for
writing a description of the C implementation of the bootloader.
Break the description up into parts,
one for each person in your group.
Each person should write a paragraph
describing the part of the implementation assigned to them.
**Collate your descriptions, and hand in the completed writeup to the CA.**

Here is a helpful diagram as you look through the code.

![bootloader diagram](images/bootloader.jpg)

### 4. Stack

Change to the directory `lab4/code/simple`. The `simple.c` program reprises a program that was used in lab3 to experiment with gdb. We will use this same program to study the use of stack memory and organization of stack frames.

Run `simple.elf` under the gdb simulator. Disassemble the `abs` function and read through its assembly instructions.

```
(gdb) disass abs
Dump of assembler code for function abs:
=> 0x00008010 <+0>:     mov r12, sp
   0x00008014 <+4>:     push {r11, r12, lr, pc}
   0x00008018 <+8>:     sub r11, r12, #4
   0x0000801c <+12>:    cmp r0, #0
   0x00008020 <+16>:    rsblt   r0, r0, #0
   0x00008024 <+20>:    sub sp, r11, #12
   0x00008028 <+24>:    ldm sp, {r11, sp, lr}
   0x0000802c <+28>:    bx  lr
End of assembler dump.
```

The first three instructions comprise the function _prolog_ which sets up the
stack frame. The last three instructions are the function _epilog_ which tears down the stack frame and restores caller-owned registers. The basic structure of the prolog and epilog is common to all functions, with some variation due to differences in local variables or use of caller-owner registers. 

Get together with your partner and carefully trace through instructions in the prolog and epilog of `abs`. Sketch a diagram of the stack frame that it creates.  Below are some issues to note and questions to discuss as you work through it.

__Function prolog__:
- Register `r11` is used as the frame pointer `fp`. The disassembly from gdb refers to `r11`, while an objdump listing calls it `fp`. The two are completely synonymous, so don't let that trip you up.
- Which four registers are pushed to the stack to set up the
APCS frame?  
- Three of the four registers in the APCS frame are caller-owned registers whose values are preserved in the prolog and later restored in the epilog. Which three are they? Which register is the fourth? Why is that register handled differently?  What is even the purpose of pushing that register as part of the APCS frame?
- What instruction in the prolog anchors `fp` to point to the current frame?
- To what location in the stack does the `fp` point?
- The first instruction of the prolog copies `sp` to `r12` and then uses `r12` in the push that follows. This dance may seems roundabout, but is unavoidable. Do you have an idea why? (Hint: a direct reference to `sp` in a push/pop instruction that itself is changing `sp` sounds like trouble...)

__Function epilog__:
- To what location in the stack does the `sp` point during the body of the function?  
- The first instruction of the epilog changes the `sp`. To what location does it pointer after executing that instruction?
- The `ldm` instruction ("load multiple") reads a sequence of words starting at a base address in memory and stores the words into the named registers. `pop` is a specific variant of `ldm` that additionally adjusts the base address as a side effect (i.e. changes stack pointer to "remove" those words) . The `ldm` instruction used in the epilog of `abs` copies three words in memory starting at the stack pointer into the registers `r11`, `sp`, and `lr`.  This effectively restores the registers to the value they had at time of function entry. The instruction does not similarly restore the value of the `pc` register. Why not?
- Which registers/memory values are updated by the `bx` instruction? 


Here is [a memory diagram when stopped at line 5 in simple.c](images/stack_abs.html). This is in the body of the `abs` function, after the prolog and before the epilog.
Our diagram shows the entire address space of the `simple`
program, including the text, data, and stack segments.  Studying this diagram will be helpful to confirm your understanding of how the stack operates and what is stored where in the address
space. 

The diagram contains a lot of details and can be overwhelming, but if you take the time to closely inspect it, you will gain a more complete understanding of the relationship between the contents of memory, registers, and the executing program.   Go over it with your partner and labmates and ask questions of each other until everyone has a clear picture of how memory is laid out.

Once you understand the prolog/epilog of `abs`, use gdb to
examine the disassembly for `diff` and `main`. 
Identify what part of the prolog and
epilog are common to all three functions and where they differ. What is the reason for those differences?

Lastly, disassemble `make_array` to see how the stack is used
to store local variables.  Sketch a picture of its stack frame as you follow along with the function instructions.

- After the instructions for the standard prolog, what additional instruction makes space for the array local variable?
- How are the contents for the array initialized (or not)? 
- In the body of the function, the array elements stored on the stack are accessed `fp`-relative.  What is the relative offset from the `fp` to the base of the array? How can you determine that from reading the assembly instructions?
- The prolog has an additional instruction to allocate space for the array, but the epilog does not seem to have a corresponding instruction to  deallocate the space. How then is the stack pointer adjusted to remove any local variables on function exit?

Compare your sketch to this [stack diagram for make_array](images/stack_makearray.html). Does your understanding line up?

### 5. Heap

Change to the directory `lab4/code/heapclient` to begin your foray in heap allocation.
So far we have stored our data either as local variables on the stack or global variables in the data segment. The functions `malloc` and `free`  offer another option, this one with more precise control of the size and lifetime and greater versatility at runtime.

Study the program `string_main.c`. The `tokenize` function is used to 
dissect a string into a sequence of space-separated tokens. The function calls on the not-yet-implemented function `char *strndup(const char *src, size_t n)` to make a copy of each token. The intended behavior of `strndup` is to return a new string containing the first `n` characters of the `src` string.

Talk over with your partner why it would not be correct for `strndup` to declare a local array variable in its stack frame to store the new string.  When a function exits, its stack frame is deallocated and the memory is recycled for use by the next function call.  What would be the consequence if `strndup` mistakenly returns a pointer to memory contained within its to-be-deallocated stack frame?

Instead `strndup` must allocate space from the heap, so that the data can persist after the function exits. Edit `strndup` to use a call to `malloc` to request the necessary number of bytes. How many bytes are needed to store a string with `n` characters?  

Now that you have the necessary memory set aside, what function from the `strings` module can you call to copy the first `n` characters from the `src` string to the new memory?

What is the final step you must take to complete the new string? (Hint: how is the end of a string marked?)

Once you have completed your implementation of `strndup` to make a proper heap copy of the string, build and run the program to verify your code is correct.

The current version of `main` stores the new strings into an array 
that is allocated on the stack. Change `main` to instead allocate this array in the heap. Make sure that your `malloc` 
requests are expressed in number of __bytes__ to allocate. Once the space is allocated (whether as a stack array or pointer to a heap array), note that access to it is functionally identical, array indexing for the win!

How many 
bytes would you need for an array of `n` chars? What if the array
elements were `int` instead of `char`? 

Unlike stack and global memory, which is automatically deallocated on your behalf, you must explicitly free dynamic memory when you are done with it. For the finishing touch, edit `main` to add the necessary calls to `free` to properly deallocate all of the heap memory it used.

## Check in with TA

At the end of the lab period, call over a TA to [check in](checkin) with your progress on the lab.

It's okay if you don't completely finish all of the exercises during
lab time; your sincere participation for the full lab period is sufficient
for credit.  If you don't make it through the whole lab, we still highly encourage you to
go through those parts, so you are well prepared to tackle the assignment.
