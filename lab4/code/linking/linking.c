#include "timer.h"

int n;
int n_init = 1;
const int n_const_init = 2;

static int n_static;
static int n_static_init = 3;
static const int n_static_const;
static const int n_static_const_init = 4;

// const int n_array[10];

static int sum(int arr[], int n)
{
    int result = 0;
    for (int i = 0; i < n; i++)
        result += arr[i];
    return result;
}

static int compute(int a, int b)
{
    int number = 9;
    int stack_array[] = {1, 5, 7, 3, 9};
    n = a;
    n_init = b + number;
    n_static += sum(stack_array, 5);
    n_static_init += a-b;

    return n + n_init + n_const_init +
    	n_static + n_static_init + n_static_const + n_static_const_init;
}

int main(void)
{
    timer_init();
    timer_delay(1);
    return compute(5, 6);
}
