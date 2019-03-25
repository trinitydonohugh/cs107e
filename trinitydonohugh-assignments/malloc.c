/*
 * File: malloc.c
 * --------------
 * This is the simple "bump" allocator from lecture.
 * An allocation request is serviced by tacking on the requested
 * space to the end of the heap thus far. 
 * It does not recycle memory (free is a no-op) so when all the
 * space set aside for the heap is consumed, it will not be able
 * to service any further requests.
 *
 * This code is given here just to show the very simplest of
 * approaches to dynamic allocation. You are to replace this code
 * with your own heap allocator implementation.
 */

#include "malloc.h"
#include "printf.h"
#include <stddef.h> // for NULL
#include "strings.h"

extern int __bss_end__;

#define STACK_START 0x8000000
#define STACK_SIZE  0x1000000
#define STACK_END ((char *)STACK_START - STACK_SIZE)

/*
 * The pool of memory used for the heap starts at the top end of the 
 * data section and extends to the bottom end of the stack.
 * It uses symbol __bss_end__ from memmap to locate data end
 * and calculates stack end assuming a 16MB stack.
 *
 * Global variables for the bump allocator:
 *
 * `heap_end` tracks location of next available address in heap
 * `heap_max` tracks location where heap is exhausted
 * 
 */
static void *heap_end = &__bss_end__;
static void *heap_max = STACK_END;


// Simple macro to round up x to multiple of n.
// The efficient but tricky bitwise approach it uses
// works only if n is a power of two -- why?
#define roundup(x,n) (((x)+((n)-1))&(~((n)-1)))

//declare block header
struct header {
    size_t payload_size;
    int status;       // 0 if free, 1 if in use
};

void *malloc (size_t nbytes)
{
    nbytes = roundup(nbytes, 8); 
    if ((char *)heap_end + nbytes + 8 > (char *)heap_max) {
        return NULL;
    }

    // declare header variables
    struct header *blockMetaData;
    int headerSize = 8;

    // traversing through heap to see if there's an empty block
    void *heap_start = &__bss_end__;
    while ((char *)heap_start < (char *)heap_end) {
        blockMetaData = (struct header *)heap_start;
        size_t sizeOfPayload = blockMetaData->payload_size;
        int status = blockMetaData->status;
        if (status == 0 && sizeOfPayload == nbytes) {
            blockMetaData->payload_size = nbytes;
            blockMetaData->status = 1;
            return blockMetaData + 1;
        } 
        else if (status == 0 && (sizeOfPayload) > (nbytes + 9)) {
            blockMetaData->payload_size = nbytes;
            blockMetaData->status = 1;
            //add new header to indicate free for rest of block
            void* tempHeap_start = heap_start;
            tempHeap_start = (char *)tempHeap_start + (int)nbytes + headerSize;
            blockMetaData = (struct header *)tempHeap_start;
            int blocksizeNewBlockDifference = sizeOfPayload-nbytes;
            blockMetaData->payload_size = blocksizeNewBlockDifference;
            blockMetaData->status = 0;
            return blockMetaData + 1;
        }
        else {
            blockMetaData = (struct header *)heap_end;
            blockMetaData->payload_size = nbytes;
            blockMetaData->status = 1;
            heap_end = (char *)heap_end + (int)nbytes + headerSize;
            return blockMetaData + 1; //pointer to payload
        }
        heap_start = (char *)heap_start + sizeOfPayload + headerSize;
    }
    blockMetaData = (struct header *)heap_end;
    blockMetaData->payload_size = nbytes;
    blockMetaData->status = 1;
    heap_end = (char *)heap_end + nbytes + headerSize;
    return blockMetaData + 1; //pointer to payload

}

void free (void *ptr)
{
    // free ptr
    void *heap_start = (struct header *)(ptr - 8);
    struct header *startBlockHeader = (struct header *)heap_start;
    startBlockHeader->status = 0;
    size_t startPayload_size = startBlockHeader->payload_size;

    // declare and gather information for while loop
    int headerSize = 8;
    void *nextBlockPtr = (char *)heap_start + startPayload_size + headerSize;
    struct header *nextBlockHeader = (struct header *) nextBlockPtr;
    int nextPtrStatus = nextBlockHeader->status;

    while (nextPtrStatus == 0 && nextBlockPtr != heap_end) {
        // edit header size of startBlockHeader
        size_t nextPayload_size = nextBlockHeader->payload_size;
        startPayload_size = startPayload_size + nextPayload_size + headerSize;
        startBlockHeader->payload_size = startPayload_size;

        // increment nextBlockHeader to point to next block
        nextBlockPtr = (char *)nextBlockPtr + nextPayload_size + headerSize;
        nextBlockHeader = (struct header *)nextBlockPtr;
        nextPtrStatus = nextBlockHeader->status;
    }
}

void *realloc (void *orig_ptr, size_t new_size)
{
    if (orig_ptr == NULL) {
        void *new_ptr = malloc(new_size);
        if (!new_ptr) {
            return NULL;
        }
        memcpy(new_ptr, orig_ptr, new_size);
        return new_ptr;
    }
    
    free(orig_ptr);

    // declare variables and read header information
    void *memToReallocate = (struct header *)(orig_ptr - 8);
    struct header *memToReallocateHeader = (struct header *)memToReallocate;
    size_t free_size = memToReallocateHeader->payload_size;

    if (free_size >= new_size) {
        memToReallocateHeader->payload_size = new_size;
        memToReallocateHeader->status = 1;
        return orig_ptr;
    } 
    else {
        void *new_ptr = malloc(new_size);
        if (!new_ptr) {
            return NULL;
        }
        memcpy(new_ptr, orig_ptr, new_size);
        return new_ptr;
    }
}

void heap_dump (const char *label)
{
    printf("\n---------- HEAP DUMP (%s) ----------\n", label);
    void *heap_start = &__bss_end__;
    struct header *blockMetaData;
    int headerSize = 8;

    while (heap_start < heap_end) {
        blockMetaData = (struct header *)heap_start;
        size_t sizeOfBlock = blockMetaData -> payload_size;
        printf("size:%d", sizeOfBlock);
        int status = blockMetaData -> status;
        printf("status:%d\n", status);
        heap_start = (char *)heap_start + (unsigned int)sizeOfBlock + headerSize;
    }

    // void *ptr = (unsigned int *)heap_end - 1;
    // printf("Heap_end: %p: %08x\n", heap_end);
    // while (ptr >= &__bss_end__) {
    //     printf("%p: %08x\n", ptr, *(unsigned int *)ptr);
    //     ptr = (unsigned int *)ptr - 1;
    // }
    printf("----------  END DUMP (%s) ----------\n", label);
}

void memory_report (void)
{
    printf("\n=============================================\n");
    printf(  "         Mini-Valgrind Memory Report         \n");
    printf(  "=============================================\n");
    // TODO: fill in for extension
}
