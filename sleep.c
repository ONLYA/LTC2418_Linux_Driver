/**
 * Cross-platform milli-second sleep function
 * Credit: https://stackoverflow.com/a/28827188/13406850
 *
 * License: CC BY-SA 4.0
 **/

#include "sleep.h"

#ifdef WIN32
#include <windows.h>
#define _SLEEP_ENVIRONMENT_ 0
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#define _SLEEP_ENVIRONMENT_ 1
#else
#include <unistd.h> // for usleep
#define _SLEEP_ENVIRONMENT_ 2
#endif

void sleep_ms(int milliseconds){ // cross-platform sleep function
#if _SLEEP_ENVIRONMENT_ == 0
    Sleep(milliseconds);
#elif _SLEEP_ENVIRONMENT_ == 1
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    if (milliseconds >= 1000)
      sleep(milliseconds / 1000);
    usleep((milliseconds % 1000) * 1000);
#endif
}