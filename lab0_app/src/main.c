#include <zephyr/kernel.h>
#include "sum.h"

int main(void)
{
    while (1) {
        sum_output(1, 2);
        k_sleep(K_SECONDS(2));
    }

    return 0;
}