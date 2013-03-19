#ifndef TIME_H
#define TIME_H

#include "output.h"
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#ifdef __APPLE__
#include <mach/clock.h>
#include <mach/mach.h>
#endif

inline void getTime(struct timespec *time)
{
#ifdef __APPLE__ // OS X does not have clock_gettime, use clock_get_time.
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    time->tv_sec = mts.tv_sec;
    time->tv_nsec = mts.tv_nsec;
#else
    if (clock_gettime(CLOCK_REALTIME, time) == -1)
        error("ERROR on gettime");
#endif
}

#endif
