#include <time.h>

#include <STI.h>
#include <unity.h>

// -----------------------------------------------------------------------------

void setUp(void)
{
    // nothing to do here
}

void absolute_timers(void);
void relative_timers(void);
void printing_timers(void);

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(absolute_timers);
    RUN_TEST(relative_timers);
    RUN_TEST(printing_timers);
    return UNITY_END();
}

void tearDown(void)
{
    // nothing to do here
}

// -----------------------------------------------------------------------------

const int64_t S_TO_uS = 1000 * 1000; // seconds to microseconds
const int64_t S_TO_mS = 1000;        // seconds to milliseconds
const int64_t mS_TO_uS = 1000;       // milliseconds to microseconds

void absolute_timers(void)
{
    TEST_ASSERT_INT64_WITHIN(2 * S_TO_uS, time(NULL) * S_TO_uS, STI_micros());
    TEST_ASSERT_INT64_WITHIN(2 * S_TO_mS, time(NULL) * S_TO_mS, STI_millis());
}

void relative_timers(void)
{
    STI_timer_t timer;
    const STI_millis_t SLEEP_mS = 250;
    const STI_millis_t MARGIN_mS = 50;
    const STI_millis_t SLEEP_uS = SLEEP_mS * mS_TO_uS;
    const STI_millis_t MARGIN_uS = MARGIN_mS * mS_TO_uS;
    STI_timer_start(&timer);
    STI_sleep_millis(SLEEP_mS);
    TEST_ASSERT_INT64_WITHIN(MARGIN_uS, SLEEP_uS, STI_elapsed_micros(timer));
    TEST_ASSERT_INT64_WITHIN(MARGIN_mS, SLEEP_mS, STI_elapsed_millis(timer));
}

void printing_timers(void)
{
    const STI_micros_t MICROS_REF = 1234567890123456; // UTC 2009-02-13 23:31:30.123456
    const STI_millis_t MILLIS_REF = MICROS_REF / 1000;
    const STI_micros_t NEG_MICROS_REF = -123456789123456; // UTC 1966-02-02 02:26:51.123456

    STI_PO_t po;
    STI_PO_set_default(&po);

    const size_t BUF_SIZE = 200;
    char buf[BUF_SIZE];

    STI_print_micros(buf, BUF_SIZE, MICROS_REF - STI_time_zone_offset(), &po);
    TEST_ASSERT_EQUAL_STRING("+ 2009/02/13 - 23:31:30.123456", buf);

    po.general.use_local_time_zone = false;
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("+ 2009/02/13 - 23:31:30.123456", buf);

    po.general.negative_means_pre_epoch = false;
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("+ 2009/02/13 - 23:31:30.123456", buf);
    STI_print_millis(buf, BUF_SIZE, MILLIS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("+ 2009/02/13 - 23:31:30.123000", buf);

    po.sign.postfix = "qwe";
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("+qwe2009/02/13 - 23:31:30.123456", buf);
    po.sign.show = false;
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("2009/02/13 - 23:31:30.123456", buf);
    po.date.separator = "__";
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("2009__02__13 - 23:31:30.123456", buf);
    po.date.postfix = "**";
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("2009__02__13**23:31:30.123456", buf);
    po.date.show = false;
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("23:31:30.123456", buf);
    po.hours_minutes.separator = "$$";
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("23$$31:30.123456", buf);
    po.hours_minutes.postfix = "££";
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("23$$31££30.123456", buf);
    po.hours_minutes.show = false;
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("30.123456", buf);
    po.seconds.postfix = " iii";
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("30.123456 iii", buf);
    po.seconds.show_micros = false;
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("30.123 iii", buf);
    po.seconds.show_millis = false;
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("30 iii", buf);
    po.seconds.show = false;
    STI_print_micros(buf, BUF_SIZE, MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("", buf);

    STI_PO_set_default(&po);
    STI_print_micros(buf, BUF_SIZE, NEG_MICROS_REF, &po);
    TEST_ASSERT_EQUAL_STRING("+ \?\?\?\?/\?\?/\?\? - \?\?:\?\?:50.876544", buf);
}