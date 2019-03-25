// File: codegen.c
// ---------------
// C examples that demo some interesting properties/transformations
// when C compiler generates ARM instructions.

// Note: some passages set a variable from an (uninitialized) global.
// If instead we had init to a constant, gcc is so smart that
// it will optimize away the rest of the work, so this
// is here to trick the compiler to treat as unknown. 

int global;

// Part (a): arithmetic
// Study each of the C statements and the translation to ARM assembly
// line 1: ARM has no negate instruction -- what is used instead?
// line 2: What is used in place of multiply for this expression?
// line 3: Note that some parts of the complex arithmetic expression 
// are pre-computed by the compiler, and other parts are not. 
// Why the difference?  Change the initialization of num to
// a constant value, e.g. int num = 3. Now how much of the
// expression is the compiler able to precompute?
void part_a(void)
{
    int num = global;

    num = -num;
    num = 5 * num;
    num =  (num & ~(-1 << 4)) * (((12*18 - 1)/2) + num); 
    global = num;
}

// Part (b): if/else
// An if/else is typically implemented in assembly as two
// distinct code paths and a control flow that routes to one 
// path or the other using branch. However, compiling the
// code below as-is shows that for a short if-then-else,
// gcc prefers use of one combined sequence of conditionally
// executed instructions to avoid the (expensive) branch.
// Edit the code to add further statements in the body of the 
// "then" to find out how long the sequence has to be to
// cause gcc to rewrite into the more tranditional branch form.
void part_b(void)
{
    int num = global;

    if (num < 0) {
        num++;
    } else {
        num--;
    }  
    global = num;
} 

// Part (c): loops
// This function is a C version of the delay loop we used in
// the blink program.  When using flag -Og,
// the compiler's generated assembly is a close match to our
// hand-written assembly. But if you change flags to -O2
// (aggressive optimization), the generated assembly is
// significantly different. Why? What can you change in the
// C code to prevent that undesired transformation, even at
// an aggressive level of optimization?
void part_c(void)
{
    int delay = 0x3f00;
    while (--delay != 0) ;
}

// Part (d): pointers
// The function below accesses memory using pointer/array
// notation. The code is divided into pairs of statements
// which perform a similar operation, but have subtle differences 
// (such as adding a typecast, or operating on pointer of 
// different pointee type). How do those differences affect
// the generated assembly? 
void part_d(void)
{
    int *ptr = &global;
    char *cptr = 0;
    int val = 107;
    int n = global;

    *ptr = val;      // compare to next ine
    *(char *)ptr = val;

    ptr[10] = val;   // compare to next line
    cptr[10] = val;

    n = *(ptr + n); // compare to next line
    n = ptr[n];
    global = n;
}


void main(void)
{
    // make calls to each function so none eliminated as dead code/unreachable
    part_a();
    part_b();
    part_c();
    part_d();
}

