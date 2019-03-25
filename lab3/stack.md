---
title: 'Background reading on the stack'
---

### APCS full frame stack layout
In Friday's lecture, we discussed the basic operation of the stack and push/pop instructions, but we ran short of reviewing the layout of a stack frame and specifically the APCS "full frame" variant. You can see the diagrams I intended to show at the tail end of Friday's [lecture slides](/lectures/Functions/slides.pdf).

The stack is used to hold information about the sequence of function calls currently executing. A function uses `push` to store a value onto the stack and `pop` to retrieve the saved value. A value being saved may be a caller-owned register or intermediate result. Large local variables (arrays and structs) too big to store in a register are also stored on the stack, but are more typically accommodated by adjusting the `sp` up and down by the needed amount than a sequence of push/pop instructions. All data stored on the stack by a function is collectively referred to its "stack frame". The compiler has latitude in what it stores on the stack; there is no guarantee about what values are stored in each frame and how values are laid out. This makes it challenging if trying to ascertain where each stack frame starts and stops.

Telling the compiler to use the APCS "full frame" variant enforces some order on the stack frame layout. The first four values pushed by a function are required to be the values of the four registers `fp`, `sp`, `lr`, and `pc`. These four saved registers form the "full frame". In addition, the register `r11` is used a dedicated "frame pointer" (`fp`). The `fp` is set to point to the first word pushed in the current stack frame (contrast to `sp` which points to the last word pushed). The `fp` tells you where to access the full frame of the currently executing function. Reading the saved fp within the full frame tells you where to access the full frame of the caller. There is a sequence of similar links leading all the way back to the main function. This chain is followed by the debugger to produce a backtrace of the current call stack.

### Stack intuition
Functions often need space. For example, they allocate 
variables or store the return
address of their caller before calling another function. There's
nothing special about this space, and we could allocate it as we would
any other memory.  However, functions calls are frequent, so we want
them as fast as possible.  Fortunately, function calls have two important
properties we can exploit for speed:

1. When functions return, all allocated memory is no longer usable.
2. Functions return in Last-In-First-Out (LIFO) order.

To optimize for speed, people have converged on using a contiguous region of memory
(called a *stack* because it's LIFO, like a stack of plates).  Roughly speaking it
works as follows:

+ At program start we allocate a fixed-sized region to hold the stack and set a
pointer (the stack pointer) to the start of that region.
+ The memory the function needs is allocated contiguously by simply adjusting
the stack pointer.
+ When a function returns, it frees the allocated memory
all-at-once by returning the stack pointer to its position when the function
was called.

Note that the stack on our system operates as a _descending_
stack. This means the stack pointer is initialized to at the highest
address.  Code decrements the stack pointer to make space for
a function call and increments the stack pointer when that function
call returns. It looks like this:

![stack picture](../images/stack.png)

This organization is so effective that compilers explicitly
support it (and do the stack pointer increment and decrement) and
architecture manuals provide the rules for how to do so.
If you know `malloc()` and `free()`, one way to compare this method
and those functions is that the fastest `malloc()` is incrementing a pointer
and the fastest `free()` is decrementing a pointer.