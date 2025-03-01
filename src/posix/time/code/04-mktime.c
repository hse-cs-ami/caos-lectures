#include <time.h>
#include <stdio.h>

int main() {
    struct tm tm = {
        .tm_year = 125,
        .tm_mon = 2,
        .tm_mday = 60,
        .tm_hour = 0,
        .tm_min = 0,
        .tm_sec = 0,
        .tm_isdst = -1,
    };
    time_t t = mktime(&tm);
    puts(ctime(&t));
    printf("isdst: %d\n", tm.tm_isdst);
}