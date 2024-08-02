#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdbool.h>

#include "../STI.h"

#define TEST_PASS 0
#define TEST_FAIL -1

#define assert_equal_or_bigger(x, ref) \
    if (x < ref)                       \
    return TEST_FAIL

#define assert_smaller_or_equal(x, ref) \
    if (x > ref)                        \
    return TEST_FAIL

int absolute_timers(STI_micros_t reference_micros)
{
    STI_micros_t actual_micros = STI_micros();
    STI_millis_t actual_millis = STI_millis();

    STI_micros_t diff_micros = reference_micros - actual_micros;
    STI_micros_t acceptable_diff_micros = 1 * 1000 * 1000; // one second
    assert_equal_or_bigger(diff_micros, -acceptable_diff_micros);
    assert_smaller_or_equal(diff_micros, acceptable_diff_micros);

    STI_millis_t reference_millis = reference_micros / 1000;
    STI_millis_t diff_millis = actual_millis - reference_millis;
    STI_millis_t acceptable_diff_millis = acceptable_diff_micros / 1000;

    assert_equal_or_bigger(diff_millis, -acceptable_diff_millis);
    assert_smaller_or_equal(diff_millis, acceptable_diff_millis);

    return TEST_PASS;
}

int relative_timers(void)
{
#define MARGIN_MILLIS 400                    // ms
#define MARGIN_MICROS (MARGIN_MILLIS * 1000) // us
#define SLEEP_MILLIS 1000                    // ms
#define SLEEP_MICROS (SLEEP_MILLIS * 1000)   // ms

    STI_timer_t timer;
    STI_timer_start(&timer);
    STI_micros_t micros;
    STI_millis_t millis;

    micros = STI_elapsed_micros(timer);
    millis = STI_elapsed_millis(timer);
    assert_equal_or_bigger(micros, 0);
    assert_smaller_or_equal(micros, 0 + MARGIN_MICROS);
    assert_equal_or_bigger(millis, 0);
    assert_smaller_or_equal(millis, 0 + MARGIN_MILLIS);

    STI_sleep_millis(SLEEP_MILLIS);

    micros = STI_elapsed_micros(timer);
    millis = STI_elapsed_millis(timer);
    assert_equal_or_bigger(micros, SLEEP_MICROS);
    assert_smaller_or_equal(micros, SLEEP_MICROS + MARGIN_MICROS);
    assert_equal_or_bigger(millis, SLEEP_MILLIS);
    assert_smaller_or_equal(millis, SLEEP_MILLIS + MARGIN_MILLIS);

    return TEST_PASS;
}

int printing(void)
{
#define BUFFER_SIZE 200
    char buffer[BUFFER_SIZE];
    STI_micros_t reference_micros = 1722533587123456;
    const char expected_micros[] = "+ 2024/08/01 - 19:33:07.123456";
    const char expected_millis[] = "+ 2024/08/01 - 19:33:07.123000";
    STI_print_micros(buffer, BUFFER_SIZE, reference_micros, NULL);
    if (strcmp(buffer, expected_micros) != 0)
    {
        return TEST_FAIL;
    }
    memset(buffer, 0, BUFFER_SIZE);
    STI_millis_t reference_millis = reference_micros;
    reference_millis /= (STI_millis_t)1000;
    STI_print_millis(buffer, BUFFER_SIZE, reference_millis, NULL);
    if (strcmp(buffer, expected_millis) != 0)
    {
        return TEST_FAIL;
    }
    return TEST_PASS;
}

#define run(test)          \
    if (test == TEST_PASS) \
    {                      \
        printf("PASS.\n"); \
    }                      \
    else                   \
    {                      \
        success = false;   \
        printf("FAIL.\n"); \
    }

int main(int argc, char **argv)
{
    bool success = true;
    printf("Test started.\n");

    STI_micros_t reference_micros = 0;
    if (argc > 1)
        sscanf(argv[1], "%" SCNi64, &reference_micros);

    printf("Absolute timers test: ");
    run(absolute_timers(reference_micros));

    printf("Relative timers test: ");
    run(relative_timers());

    printf("Printing test: ");
    run(printing());

    if (success)
    {
        printf("All tests finished successfully.\n");
        return TEST_PASS;
    }
    printf("Test FAILED.\n");
    return TEST_FAIL;
}
