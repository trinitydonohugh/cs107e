#include "assert.h"
#include "backtrace.h"
#include "malloc.h"
#include "../nameless.h"
#include "rand.h"
#include <stdint.h>
#include "strings.h"
#include "uart.h"

void main(void);
void heap_dump(const char *label);  // Debugging output from malloc module

static void test_backtrace_simple(void)
{
    frame_t f[3];
    int frames_filled = backtrace(f, 3);

    assert(frames_filled == 3);
    assert(strcmp(f[0].name, "test_backtrace_simple") == 0);
    assert(f[0].resume_addr == (uintptr_t)test_backtrace_simple + f[0].resume_offset);
    assert(strcmp(f[1].name, "main") == 0);
    assert(f[1].resume_addr == (uintptr_t)main + f[1].resume_offset);
    printf("Here is a simple backtrace:\n");
    print_frames(f, frames_filled);
    printf("\n");
}

static int recursion_fun(int n)
{
    if (n == 0)
        return mystery();   // look in nameless.c
    else
        return 1 + recursion_fun(n-1);
}

static int test_backtrace_complex(int n)
{
    return recursion_fun(n);
}

static void test_heap_dump(void)
{
    heap_dump("Empty heap");

    char *p = malloc(8);
    heap_dump("After p = malloc(8)");

    char *p1 = malloc(16);
    heap_dump("After p1 = malloc(16)");

    free(p);
    heap_dump("After free(p)");

    char *p2 = malloc(24);
    heap_dump("After p2 = malloc(24)");

    char *p3 = malloc(8);
    heap_dump("After p3 = malloc(8)");

    char *p4 = malloc(30);
    heap_dump("After p3 = malloc(30)");

    free(p2);
    heap_dump("After free(p2)");

    free(p1);
    heap_dump("After free(p1)");

    free(p4);
    heap_dump("After free(p4)");

    char *p5 = malloc(0);
    heap_dump("After malloc(0)");

    free (NULL);
    heap_dump("After free (NULL)");

    p = realloc(p, 32);
    heap_dump("After p = realloc(p, 32)");

    p2 = realloc(NULL, 24);
    heap_dump("After p = realloc(NULL, size)");

    p3 = realloc(p3, 0);
    heap_dump("After p = realloc(ptr, 0)");
}

static void test_heap_simple(void)
{
    char *s;

    s = malloc(6);
    memcpy(s, "hello", 6);
    assert(strcmp(s, "hello") == 0);

    s = realloc(s, 12);
    strlcat(s, " world", 12);
    assert(strcmp(s, "hello world") == 0);
    free(s);
}

// array of dynamically-allocated strings, each
// string filled with repeated char, e.g. "A" , "BB" , "CCC"
// Examine each string, verify expected contents intact.
static void test_heap_multiple(void)
{
    int n = 26;
    char *arr[n];

    for (int i = 0; i < n; i++) {
        int num_repeats = i + 1;
        char *ptr = malloc(num_repeats + 1);
        assert(ptr != NULL);
        assert((uintptr_t)ptr % 8 == 0); // verify 8-byte alignment
        memset(ptr, 'A' - 1 + num_repeats, num_repeats);
        ptr[num_repeats] = '\0';
        arr[i] = ptr;
    }
    for (int i = n-1; i >= 0; i--) {
        int len = strlen(arr[i]);
        char first = arr[i][0], last = arr[i][len -1];
        assert(first == 'A' - 1 + len);
        assert(first == last);
        free(arr[i]);
    }
}

#define max(x, y) ((x) > (y) ? (x) : (y))

static void test_heap_recycle(int niter)
{
    extern int __bss_end__;
    void *heap_low = &__bss_end__, *heap_high = NULL;

    size_t max_size = 1024, total = 0;
    void *p = malloc(1);

    for (int i = 0; i < niter; i++) {
        size_t size = rand() % max_size;
        void *q = malloc(size);
        p = realloc(p, size);
        total += 2*size;
        void *higher = (char *)max(p, q) + size;
        heap_high = max(heap_high, higher);
        free(q);
    }
    free(p);
    size_t extent = (char *)heap_high - (char *)heap_low;
    size_t percent = total > extent ? (100*total)/extent : 0;
    printf("\nRecycling report:\n");
    printf("Completed %d iterations. Allocated %d bytes. Heap extent %d bytes. Recycling = %d%%\n", niter, total, extent, percent);
}

void test_heap_redzones(void)
{
    // DO NOT ATTEMPT THIS TEST unless your heap has red zone protection!
    char *ptr;

    ptr = malloc(9);
    memset(ptr, 'a', 9); // write into payload
    free(ptr); // ptr is OK

    ptr = malloc(5);
    ptr[-1] = 0x45; // write before payload
    free(ptr);      // ptr is NOT ok

    ptr = malloc(12);
    ptr[13] = 0x45; // write after payload
    free(ptr);      // ptr is NOT ok
}


void main(void)
{
    uart_init();

    test_backtrace_simple();
    test_backtrace_simple(); // Again so you can see the main offset change!
    test_backtrace_complex(7);  // Slightly tricky backtrace

    test_heap_dump();

    test_heap_simple();
    test_heap_multiple();
    test_heap_recycle(10);
    
    //test_heap_redzones(); // DO NOT USE unless you implemented red zone protection

    printf("\nProgram %s completed.\n", __FILE__);
}
