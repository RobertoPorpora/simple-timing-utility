# STI: Simple Timing Utility

The **STI** library provides simple and cross-platform timing and date-time printing utilities for C.  
It includes functions for handling absolute and relative timers, as well as formatting time for printing.

## Features

- **Absolute Timers:** Retrieve current time (since EPOCH) in microseconds or milliseconds.
- **Relative Timers:** Measure elapsed time and pause execution.
- **Printing:** Format and print time values with customizable options.

## Usage

### Absolute Timers

- `STI_micros()`
  - Returns the current time in microseconds.
  
- `STI_millis()`
  - Returns the current time in milliseconds.

### Relative Timers

- `void STI_timer_start(STI_timer_t *timer)`
  - Starts a new timer. Pass a `STI_timer_t` variable to hold the timer value.

- `STI_micros_t STI_elapsed_micros(STI_timer_t timer)`
  - Returns the elapsed time in microseconds since `STI_timer_start` was called.

- `STI_millis_t STI_elapsed_millis(STI_timer_t timer)`
  - Returns the elapsed time in milliseconds since `STI_timer_start` was called.

- `void STI_sleep_millis(STI_millis_t millis)`
  - Pauses execution for the specified number of milliseconds.

### Printing

- **Print Options Structs:**
  - `STI_PO_sign_t`
  - `STI_PO_date_t`
  - `STI_PO_hours_minutes_t`
  - `STI_PO_seconds_t`
  - `STI_PO_t`
  
  Configure how time should be printed, including options for date, hours, minutes, seconds, and more.

- `char *STI_print_micros(char *dest, size_t size, STI_micros_t micros, STI_PO_t *options)`
  - Formats and prints microseconds to `dest` using specified `options`.

- `char *STI_print_millis(char *dest, size_t size, STI_millis_t millis, STI_PO_t *options)`
  - Formats and prints milliseconds to `dest` using specified `options`.

- `void STI_print_options_set_default(STI_PO_t *options)`
  - Sets default print options.

## Example

```c
#include "sti.h"

int main() {
    // Absolute time
    STI_micros_t start = STI_micros();
    STI_millis_t start_millis = STI_millis();

    // Relative time
    STI_timer_t timer;
    STI_timer_start(&timer);
    // ... code to measure
    STI_micros_t elapsed_us = STI_elapsed_micros(timer);
    STI_millis_t elapsed_ms = STI_elapsed_millis(timer);

    // Sleep
    STI_sleep_millis(1000);

    // Printing the local time
    STI_PO_t options;
    STI_print_options_set_default(&options);
    char buffer[100];
    STI_print_micros(buffer, sizeof(buffer), start, &options);
    // buffer = "+ 2024/08/01 - 19:33:01.123456"

    // customize options to modify the layout
    options.sign.show = false;
    options.seconds.show = false;
    STI_print_micros(buffer, sizeof(buffer), start, &options);
    // buffer = "2024/08/01 - 19:33"
    options.sign.show = true;
    options.date.show = false;
    options.hours_minutes.show = false;
    STI_print_micros(buffer, sizeof(buffer), start, &options);
    // buffer = "+ 01.123456"
    
    return 0;
}
```

## Compiling

### Method 1
Use CMake.  
Copy the whole folder inside your project folder and add these to your CMakeLists.txt
- `add_subdirectory(path/to/simple_timing_utility)`
- `target_link_libraries(your_executable_name STI)`

You have, of course to customize "path/to/" and "your_executable_name" to your needs.

### Method 2
Manually copy files STI.c and STI.h inside your project folder sources.  
(And configure your compiler to compile and link these).


## Testing

First, you need gcc, make, cmake and python installed in your system, check it with these 4 commands:
```
gcc --version
make --version
cmake --version
python --version
```

Then, in order to launch the tests open a terminal with cwd as the folder of this README.md file.  

Then, run `python test/run.py`.

This will launch cmake, make, the compiler and then it will execute the executable output.

After a few seconds you should see something like this in the last lines of your terminal:
```
Test started.
Absolute timers test: PASS.
Relative timers test: PASS.
Printing test: PASS.
All tests finished successfully.


Reached end of run.py
```