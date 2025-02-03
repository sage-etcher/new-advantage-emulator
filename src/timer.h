
#ifndef TIMER_HEADER
#define TIMER_HEADER

#include <time.h>


void timespec_diff (struct timespec end, struct timespec start, 
                    struct timespec *p_diff);

void timespec_unsigned (struct timespec *time);

int timespec_equal (struct timespec time_a, struct timespec time_b);

struct timespec timer (struct timespec *previous, struct timespec rate);

#endif
