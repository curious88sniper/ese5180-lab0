#include <zephyr/logging/log.h>
#include "sum.h"

LOG_MODULE_REGISTER(sum_log, CONFIG_LOG_DEFAULT_LEVEL);

int sum(int a, int b)
{
    return a + b;
}

void sum_output(int a, int b)
{
    LOG_INF("log: %d + %d = %d", a, b, sum(a, b));
}