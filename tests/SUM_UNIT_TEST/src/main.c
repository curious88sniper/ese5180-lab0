#include <zephyr/ztest.h>
#include "sum.h"

ZTEST(sum_tests, test_sum_positive)
{
    zassert_equal(sum(1, 2), 3, "1 + 2 should equal 3");
}

ZTEST(sum_tests, test_sum_negative)
{
    zassert_equal(sum(-4, -5), -9, "-4 + -5 should equal -9");
}

ZTEST(sum_tests, test_sum_zero)
{
    zassert_equal(sum(0, 0), 0, "0 + 0 should equal 0");
}

ZTEST_SUITE(sum_tests, NULL, NULL, NULL, NULL, NULL);
