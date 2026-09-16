#include <zephyr/kernel.h>
#include "sum.h"

int sum(int a, int b)
{
    return a + b;
}

void sum_output(int a, int b)
{
    printk("printk: %d + %d = %d\n", a, b, sum(a, b));
}