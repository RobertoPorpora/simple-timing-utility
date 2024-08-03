#include <time.h>
#include <stdio.h>
#include <string.h>

#include "STI.h"

#ifdef _WIN32

#include <Windows.h>

STI_micros_t STI_micros()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER ui;
    ui.LowPart = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;
    return (ui.QuadPart / 10) - 11644473600000000ULL;
}

#define localtime_cross_platform(tm_ptr, sec) localtime_s(tm_ptr, sec)

#else

#include <sys/time.h>
#include <unistd.h>

STI_micros_t STI_micros()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (STI_micros_t)tv.tv_sec * 1000000 + (STI_micros_t)tv.tv_usec;
}

#define localtime_cross_platform(tm_ptr, sec) localtime_r(sec, tm_ptr)

#endif

STI_millis_t STI_millis()
{
    return STI_micros() / 1000;
}

// -----------------------------------------------------------------------------

void STI_timer_start(STI_timer_t *timer)
{
    *timer = STI_micros();
}

STI_micros_t STI_elapsed_micros(STI_timer_t timer)
{
    return STI_micros() - timer;
}

STI_millis_t STI_elapsed_millis(STI_timer_t timer)
{
    return STI_elapsed_micros(timer) / 1000;
}

#ifdef _WIN32

void STI_sleep_millis(STI_millis_t millis)
{
    Sleep(millis);
}

#else

void STI_sleep_millis(STI_millis_t millis)
{
    usleep(millis * 1000);
}

#endif

// -----------------------------------------------------------------------------

size_t min_size_for_sign(void)
{
    // + or -
    return 1;
}

size_t min_size_for_date(const char *separator)
{
    // YYYY<separator>MM<separator>DD
    return 8 + 2 * strlen(separator);
}

size_t min_size_for_hours_minutes(const char *separator)
{
    // HH<separator>MM
    return 4 + strlen(separator);
}

size_t min_size_for_seconds(bool print_milis, bool print_micros)
{
    // SS.MMMUUU
    if (print_micros)
        return 9;
    if (print_milis)
        return 6;
    return 2;
}

size_t min_size_needed(STI_PO_t *options)
{
    size_t out = 0;
    if (options->sign.show)
    {
        out += min_size_for_sign();
        out += strlen(options->sign.postfix);
    }

    if (options->date.show)
    {
        out += min_size_for_date(options->date.separator);
        out += strlen(options->date.postfix);
    }

    if (options->hours_minutes.show)
    {
        out += min_size_for_hours_minutes(options->hours_minutes.separator);
        out += strlen(options->hours_minutes.postfix);
    }

    if (options->seconds.show)
    {
        out += min_size_for_seconds(options->seconds.show_millis, options->seconds.show_micros);
        out += strlen(options->seconds.postfix);
    }

    return out;
}

////

#define ptr_shift(ptr, shift) ptr = &ptr[shift]

static char *print_sign(char *dest, STI_PO_sign_t *options, STI_micros_t micros)
{
    size_t offset = 0;

    if (options->show)
    {
        if (micros < 0)
            offset = sprintf(dest, "-%s", options->postfix);
        else
            offset = sprintf(dest, "+%s", options->postfix);

        ptr_shift(dest, offset);
    }

    return dest;
}

static char *print_date_hours_minutes(char *dest, STI_PO_t *options, time_t seconds)
{
    struct tm tm_info;
    localtime_cross_platform(&tm_info, &seconds);

    size_t offset = 0;
    if (options->date.show)
    {
        offset = sprintf(
            dest // destination string
            ,
            "%04d" // year
            "%s"   // separator
            "%02d" // month
            "%s"   // separator
            "%02d" // day
            "%s"   // postfix
            ,
            tm_info.tm_year + 1900,  // year
            options->date.separator, // separator
            tm_info.tm_mon + 1,      // month
            options->date.separator, // separator
            tm_info.tm_mday,         // day
            options->date.postfix    // postfix
        );

        ptr_shift(dest, offset);
    }

    if (options->hours_minutes.show)
    {
        offset = sprintf(
            dest // destination string
            ,
            "%02d" // hour
            "%s"   // separator
            "%02d" // minutes
            "%s"   // postfix
            ,
            tm_info.tm_hour,                  // hour
            options->hours_minutes.separator, // separator
            tm_info.tm_min,                   // minutes
            options->hours_minutes.postfix    // postfix
        );

        ptr_shift(dest, offset);
    }

    return dest;
}

static char *print_seconds(char *dest, STI_PO_seconds_t *options, float seconds_with_decimals)
{
    // in comments, example with input 1.234567 (1 second, 234 millis and 567 micros)
    size_t offset = 0;
    if (options->show)
    {
        if (options->show_micros)
            offset = sprintf(dest, "%09.6f", seconds_with_decimals); // 01.234567
        else if (options->show_millis)
            offset = sprintf(dest, "%06.3f", seconds_with_decimals); // 01.234
        else
            offset = sprintf(dest, "%02.0f", seconds_with_decimals); // 01
        ptr_shift(dest, offset);

        offset = sprintf(dest, "%s", options->postfix);
        ptr_shift(dest, offset);
    }
    return dest;
}

static void fill_options_sign(STI_PO_sign_t *options)
{
    static const char *DEFAULT_SIGN_POSTFIX = " ";
    if (options->postfix == NULL)
        options->postfix = DEFAULT_SIGN_POSTFIX;
}

static void fill_options_date(STI_PO_date_t *options)
{
    static const char *DEFAULT_DATE_SEPARATOR = "/";
    static const char *DEFAULT_DATE_POSTFIX = " - ";
    if (options->separator == NULL)
        options->separator = DEFAULT_DATE_SEPARATOR;
    if (options->postfix == NULL)
        options->postfix = DEFAULT_DATE_POSTFIX;
}

static void fill_options_hours_minutes(STI_PO_hours_minutes_t *options)
{

    static const char *DEFAULT_HOURS_MINUTES_SEPARATOR = ":";
    static const char *DEFAULT_HOURS_MINUTES_POSTFIX = ":";
    if (options->separator == NULL)
        options->separator = DEFAULT_HOURS_MINUTES_SEPARATOR;
    if (options->postfix == NULL)
        options->postfix = DEFAULT_HOURS_MINUTES_POSTFIX;
}

static void fill_options_seconds(STI_PO_seconds_t *options)
{
    static const char *DEFAULT_SECONDS_POSTFIX = "";
    if (options->postfix == NULL)
        options->postfix = DEFAULT_SECONDS_POSTFIX;
}

void STI_print_options_set_default(STI_PO_t *options)
{
    options->sign.show = true;
    options->sign.postfix = NULL;
    fill_options_sign(&(options->sign));

    options->date.show = true;
    options->date.separator = NULL;
    options->date.postfix = NULL;
    fill_options_date(&(options->date));

    options->hours_minutes.show = true;
    options->hours_minutes.separator = NULL;
    options->hours_minutes.postfix = NULL;
    fill_options_hours_minutes(&(options->hours_minutes));

    options->seconds.show = true;
    options->seconds.show_millis = true;
    options->seconds.show_micros = true;
    options->seconds.postfix = NULL;
    fill_options_seconds(&(options->seconds));
}

char *STI_print_micros(char *dest, size_t size, STI_micros_t micros, STI_PO_t *options)
{
    // check if destination is valid
    if (dest == NULL)
        return NULL;

    // fill options
    STI_PO_t opt;
    if (NULL != options)
    {
        memcpy(&opt, options, sizeof(STI_PO_t));
    }
    else
    {
        STI_print_options_set_default(&opt);
    }
    fill_options_sign(&(opt.sign));
    fill_options_date(&(opt.date));
    fill_options_hours_minutes(&(opt.hours_minutes));
    fill_options_seconds(&(opt.seconds));

    // check if size is enough.
    if (size < min_size_needed(&opt))
        return NULL;

    // print sign
    dest = print_sign(dest, &(opt.sign), micros);

    // remove sign
    if (micros < 0)
        micros = -micros;

    // backup value
    STI_micros_t micros_backup = micros;

    // converts micros to seconds
    micros = micros / (1000 * 1000);
    time_t seconds = micros;

    // print date, hours and minutes
    dest = print_date_hours_minutes(dest, &(opt), seconds);

    // extract seconds and decimals from previous backup
    const STI_micros_t ONE_MINUTE = 60 * 1000 * 1000;
    micros_backup = micros_backup % ONE_MINUTE;
    float seconds_with_decimals = micros_backup;
    const float MICROS_IN_ONE_SECOND = 1000 * 1000;
    // move the decimal point, example: 1234567.0 -> 1.234567
    seconds_with_decimals /= MICROS_IN_ONE_SECOND;

    // print seconds with decimals
    dest = print_seconds(dest, &(opt.seconds), seconds_with_decimals);

    // return shifted pointer
    return dest;
}

char *STI_print_millis(char *dest, size_t size, STI_millis_t millis, STI_PO_t *options)
{
    return STI_print_micros(dest, size, (STI_micros_t)millis * (STI_micros_t)1000, options);
}