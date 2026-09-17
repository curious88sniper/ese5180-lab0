#include <zephyr/logging/log.h>
#include "sum.h"

LOG_MODULE_REGISTER(sum_log, CONFIG_LOG_DEFAULT_LEVEL);

int sum(int a, int b)
{
    return a + b;
}

void sum_output(int a, int b)
{
    int inputs[] = { a, b };
    int result = sum(a, b);

    LOG_INF("sum log implementation selected");
    LOG_DBG("debug: first input=%d second input=%d", a, b);
    LOG_WRN("warning example: computing sum with logger");
    LOG_HEXDUMP_INF(inputs, sizeof(inputs), "sum inputs");
    LOG_INF("%d + %d = %d", a, b, result);
}