#include <sys/time.h>
#include <stdio.h>

/*
    struct timeval {
        time_t      tv_sec;     // seconds
        suseconds_t tv_usec;    // microseconds
    };
    int gettimeofday(struct timeval *restrict tv,
                        struct timezone *_Nullable restrict tz);
    int settimeofday(const struct timeval *tv,
                        const struct timezone *_Nullable tz);
*/

int main() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    printf("Current time: %ld.%06ld\n", tv.tv_sec, tv.tv_usec);
}