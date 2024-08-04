#include <time.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "STI.h"

#ifdef _WIN32

#include <windows.h>

STI_micros_t STI_micros()
{
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER ui;
    ui.LowPart = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;
    return (ui.QuadPart / 10) - 11644473600000000ULL;
}

#define localtime_thread_safe(tm_ptr, sec_ptr) localtime_s(tm_ptr, sec_ptr)
#define gmtime_thread_safe(tm_ptr, sec_ptr) gmtime_s(tm_ptr, sec_ptr)

#else

#include <sys/time.h>
#include <unistd.h>

STI_micros_t STI_micros()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (STI_micros_t)tv.tv_sec * 1000000 + (STI_micros_t)tv.tv_usec;
}

#define localtime_thread_safe(tm_ptr, sec_ptr) localtime_r(sec_ptr, tm_ptr)
#define gmtime_thread_safe(tm_ptr, sec_ptr) gmtime_r(se_ptr, tm_ptr)

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

static char *print_sign(char *dest, STI_PO_sign_t *options, bool is_negative)
{
    size_t offset = 0;

    if (options->show)
    {
        if (is_negative)
            offset = sprintf(dest, "-%s", options->postfix);
        else
            offset = sprintf(dest, "+%s", options->postfix);

        ptr_shift(dest, offset);
    }

    return dest;
}

#define date_error(time_info_ptr) (-1 == (time_info_ptr)->tm_mon)

static char *print_date_hours_minutes(char *dest, STI_PO_t *options, time_t seconds)
{
    struct tm time_info;

    if (options->general.use_local_time_zone)
    {
        localtime_thread_safe(&time_info, &seconds);
        if (date_error(&time_info))
        {
            gmtime_thread_safe(&time_info, &seconds);
        }
    }
    else
    {
        gmtime_thread_safe(&time_info, &seconds);
    }

    size_t offset = 0;
    if (options->date.show)
    {
        if (!date_error(&time_info))
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
                time_info.tm_year + 1900, // year
                options->date.separator,  // separator
                time_info.tm_mon + 1,     // month
                options->date.separator,  // separator
                time_info.tm_mday,        // day
                options->date.postfix     // postfix
            );
        }
        else
        {
            offset = sprintf(
                dest // destination string
                ,
                "????" // year
                "%s"   // separator
                "??"   // month
                "%s"   // separator
                "??"   // day
                "%s"   // postfix
                ,
                options->date.separator, // separator
                options->date.separator, // separator
                options->date.postfix    // postfix
            );
        }
        ptr_shift(dest, offset);
    }

    if (options->hours_minutes.show)
    {
        if (!date_error(&time_info))
        {
            offset = sprintf(
                dest // destination string
                ,
                "%02d" // hour
                "%s"   // separator
                "%02d" // minutes
                "%s"   // postfix
                ,
                time_info.tm_hour,                // hour
                options->hours_minutes.separator, // separator
                time_info.tm_min,                 // minutes
                options->hours_minutes.postfix    // postfix
            );
        }
        else
        {
            offset = sprintf(
                dest // destination string
                ,
                "??" // hour
                "%s" // separator
                "??" // minutes
                "%s" // postfix
                ,
                options->hours_minutes.separator, // separator
                options->hours_minutes.postfix    // postfix
            );
        }
        ptr_shift(dest, offset);
    }

    return dest;
}

static char *print_seconds(char *dest, STI_PO_seconds_t *options, STI_micros_t integer_part, STI_micros_t decimal_part)
{
    size_t offset = 0;
    if (options->show)
    {
        offset = sprintf(dest, "%02" PRIu64, integer_part);
        ptr_shift(dest, offset);

        if (options->show_micros)
        {
            offset = sprintf(dest, ".%06" PRIu64, decimal_part);
            ptr_shift(dest, offset);
        }
        else if (options->show_millis)
        {
            offset = sprintf(dest, ".%03" PRIu64, decimal_part / 1000);
            ptr_shift(dest, offset);
        }

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

void STI_PO_set_default(STI_PO_t *options)
{
    options->general.negative_means_pre_epoch = true;
    options->general.use_local_time_zone = true;

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

    char *initial_dest = dest;

    // fill options
    STI_PO_t opt;
    if (NULL != options)
    {
        memcpy(&opt, options, sizeof(STI_PO_t));
    }
    else
    {
        STI_PO_set_default(&opt);
    }
    fill_options_sign(&(opt.sign));
    fill_options_date(&(opt.date));
    fill_options_hours_minutes(&(opt.hours_minutes));
    fill_options_seconds(&(opt.seconds));

    // check if size is enough.
    if (size < min_size_needed(&opt))
        return NULL;

    // split parts
    const STI_micros_t MICROS_IN_ONE_SECOND = 1000 * 1000;
    const STI_micros_t MICROS_IN_ONE_MINUTE = 60 * MICROS_IN_ONE_SECOND;

    bool is_negative;
    time_t hours_minutes;
    STI_micros_t seconds;
    STI_micros_t integer_part;
    STI_micros_t decimal_part;
    if (opt.general.negative_means_pre_epoch)
    {
        is_negative = false;
        hours_minutes = micros / MICROS_IN_ONE_SECOND;
        seconds = micros % MICROS_IN_ONE_MINUTE;
        if (seconds < 0)
        {
            seconds += MICROS_IN_ONE_MINUTE;
        }
    }
    else
    {
        is_negative = micros < 0;
        if (is_negative)
        {
            hours_minutes = -(micros / MICROS_IN_ONE_SECOND);
            seconds = -(micros % MICROS_IN_ONE_MINUTE);
        }
        else
        {
            hours_minutes = micros / MICROS_IN_ONE_SECOND;
            seconds = micros % MICROS_IN_ONE_MINUTE;
        }
    }
    integer_part = seconds / MICROS_IN_ONE_SECOND;
    decimal_part = seconds % MICROS_IN_ONE_SECOND;

    dest = print_sign(dest, &(opt.sign), is_negative);
    dest = print_date_hours_minutes(dest, &(opt), hours_minutes);
    dest = print_seconds(dest, &(opt.seconds), integer_part, decimal_part);

    // if nothing has been printed, terminate the string on the first char
    if (initial_dest == dest)
        *dest = '\0';

    // return shifted pointer
    return dest;
}

char *STI_print_millis(char *dest, size_t size, STI_millis_t millis, STI_PO_t *options)
{
    return STI_print_micros(dest, size, (STI_micros_t)millis * (STI_micros_t)1000, options);
}

// -----------------------------------------------------------------------------

void STI_sleep_millis(STI_millis_t millis)
{
#ifdef _WIN32
    Sleep(millis);
#else
    usleep(millis * 1000);
#endif
}

STI_micros_t STI_time_zone_offset(void)
{
    time_t seconds = time(NULL);
    struct tm tm_info_local;
    struct tm tm_info_utc;

    localtime_thread_safe(&tm_info_local, &seconds);
    gmtime_thread_safe(&tm_info_utc, &seconds);

    STI_micros_t offset = mktime(&tm_info_local) - mktime(&tm_info_utc);
    offset *= 1000 * 1000;
    return offset;
}
