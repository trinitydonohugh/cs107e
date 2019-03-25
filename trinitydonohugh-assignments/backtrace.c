#include "backtrace.h"
#include "printf.h"

int backtrace (frame_t f[], int max_frames)
{
    //get current fp address
    unsigned int *cur_fp;
   	__asm__("mov %0, fp" : "=r" (cur_fp));

   	int frameCounter = 0;
   	while (*(cur_fp - 3) != 0x0 && frameCounter < max_frames) {
   		unsigned int *lr = *(cur_fp - 1);
   		f[frameCounter].resume_addr = lr;
   		unsigned int * callerpc = *(cur_fp - 3);
   		f[frameCounter].resume_offset = (*(cur_fp - 1)) - (*callerpc - 12);
   		unsigned int *locationOfPossibleName = (*callerpc)-16;
   		if (*locationOfPossibleName >= 0xff000000) {
   			unsigned int lengthOfName = *locationOfPossibleName & ~0xff000000;
   			f[frameCounter].name = (char *)(locationOfPossibleName - (lengthOfName/4));		
   		} 
   		else {
   			f[frameCounter].name = "???";
   		}
   		frameCounter++;
   		cur_fp = *(cur_fp - 3);
   	}
    return frameCounter;
}

void print_frames (frame_t f[], int n)
{
    for (int i = 0; i < n; i++)
        printf("#%d 0x%x at %s+%d\n", i, f[i].resume_addr, f[i].name, f[i].resume_offset);
}

void print_backtrace (void)
{
    int max = 50;
    frame_t arr[max];

    int n = backtrace(arr, max);
    print_frames(arr+1, n-1);   // print frames starting at this function's caller
}
