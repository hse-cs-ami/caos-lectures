#include <time.h>
#include <stdio.h>

int main() {
    /*
        Historical time standard: GMT (Greenwich mean time)
        Current time standard: UTC
            (CUT – coordinated universal time, TUC – temps universel coordonné)
        UTC: atomic clocks + leap seconds due to irregular rotation of the Earth

        Unix timestamp (Epoch time, POSIX time): number of seconds since 
        January 1, 1970, 00:00:00 UTC (the Unix Epoch), excluding leap seconds

        time_t time(time_t *t)
        (returns current timestamp and stores it in *t if t != NULL)

        time_t used to be 32 bits wide, leading to the "Year 2038 problem"
    */

    time_t now = time(NULL);
    printf("Unix timestamp: %ld\n", now);
    printf("sizeof(time_t): %zu\n", sizeof(now));
}