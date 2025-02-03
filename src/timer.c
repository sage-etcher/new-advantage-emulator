
#include "timer.h"

#include <assert.h>
#include <stdio.h>
#include <threads.h>
#include <time.h>


void
timespec_diff (struct timespec end, struct timespec start, 
               struct timespec *p_diff)
{
    const long ONE_SECOND = 1000000000;

    assert (p_diff);

    p_diff->tv_sec  = end.tv_sec  - start.tv_sec;
    p_diff->tv_nsec = end.tv_nsec - start.tv_nsec;

    if (p_diff->tv_sec < 0)
    {
        p_diff->tv_sec--;
        p_diff->tv_nsec += ONE_SECOND;
    }
}


void
timespec_unsigned (struct timespec *time)
{
    assert (time);

    if (time->tv_sec >= 0) { return; }

    *time = (struct timespec){
        .tv_sec = 0,
        .tv_nsec = 0,
    };
}


int
timespec_equal (struct timespec time_a, struct timespec time_b)
{
    if (time_a.tv_sec != time_b.tv_sec) { return 0; }
    if (time_a.tv_nsec != time_b.tv_nsec) { return 0; }

    return 1;
}


struct timespec
timer (struct timespec *previous, struct timespec rate)
{ 
    struct timespec current = { 0 };
    struct timespec diff = { 0 };
    struct timespec sleep_time = { 0 };

    (void)timespec_get (&current, 0);

    timespec_diff (current, *previous, &diff);  /* time passed */
    timespec_diff (rate, diff, &sleep_time);    /* time needed to sleep */

    /* if sleep is required, sleep, otherwise dont */
    if ((sleep_time.tv_sec  > 0) ||
        (sleep_time.tv_nsec > 0))
    {
        (void)thrd_sleep (&sleep_time, NULL);
#if 0
        printf ("sleep for %ld s, %ld ns\n", 
                sleep_time.tv_sec, sleep_time.tv_nsec);
    }
    else
    {
        printf ("negative time\n");
#endif
    }

    *previous = current;

    return sleep_time;
}


/* end of file */
