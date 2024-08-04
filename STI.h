#pragma once

/**
 * STI is for "Simple TIming utility"
 */

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * ABSOLUTE TIMERS
 */

typedef int64_t STI_micros_t;
typedef int64_t STI_millis_t;
typedef STI_micros_t STI_timer_t;

STI_micros_t STI_micros();
STI_millis_t STI_millis();

/**
 * RELATIVE TIMERS
 */

void STI_timer_start(STI_timer_t *timer);
STI_micros_t STI_elapsed_micros(STI_timer_t timer);
STI_millis_t STI_elapsed_millis(STI_timer_t timer);

/**
 * PRINTING
 *
 * PO is for "Print Options"
 */

typedef struct STI_PO_sign_t
{
    bool show;
    const char *postfix; // set to NULL for default value
} STI_PO_sign_t;

typedef struct STI_PO_date_t
{
    bool show;
    const char *separator; // set to NULL for default value
    const char *postfix;   // set to NULL for default value
} STI_PO_date_t;

typedef struct STI_PO_hours_minutes_t
{
    bool show;
    const char *separator; // set to NULL for default value
    const char *postfix;   // set to NULL for default value
} STI_PO_hours_minutes_t;

typedef struct STI_PO_seconds_t
{
    bool show;
    bool show_millis;
    bool show_micros;
    const char *postfix; // set to NULL for default value
} STI_PO_seconds_t;

typedef struct STI_PO_general_t
{
    bool negative_means_pre_epoch;
    bool use_local_time_zone;
} STI_PO_general_t;

typedef struct STI_PO_t
{
    STI_PO_general_t general;
    STI_PO_sign_t sign;
    STI_PO_date_t date;
    STI_PO_hours_minutes_t hours_minutes;
    STI_PO_seconds_t seconds;
} STI_PO_t;

void STI_PO_set_default(STI_PO_t *options);

char *STI_print_micros(char *dest, size_t size, STI_micros_t micros, STI_PO_t *options);
char *STI_print_millis(char *dest, size_t size, STI_millis_t millis, STI_PO_t *options);

/**
 * GENERIC HELPERS
 */

void STI_sleep_millis(STI_millis_t millis);
STI_micros_t STI_time_zone_offset(void);