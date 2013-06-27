#ifndef TIME_H
#define TIME_H

#include "output.h"
#include <time.h>
#include <sys/time.h>
#include <stdio.h>

#ifdef MAC_OS
#include <mach/clock.h>
#include <mach/mach.h>
#endif

inline timespec* getTime(timespec* time)
{
#ifdef MAC_OS // OS X does not have clock_gettime, use clock_get_time.
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    time->tv_sec = mts.tv_sec;
    time->tv_nsec = mts.tv_nsec;
#else
    if (clock_gettime(CLOCK_REALTIME, time) == -1)
        error("ERROR on gettime %s", strerror(errno));
#endif
    return time;
}

inline LL getTimeLL(struct timespec *time)
{
    return time->tv_sec * 1000LL + time->tv_nsec / 1000000;
}

#endif
