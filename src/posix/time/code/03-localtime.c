#include <time.h>
#include <stdio.h>

/*
struct tm {
    int tm_sec;     // seconds (0 - 60)
    int tm_min;     // minutes (0 - 59)
    int tm_hour;    // hours (0 - 23)
    int tm_mday;    // day of month (1 - 31)
    int tm_mon;     // month of year (0 - 11)
    int tm_year;    // year - 1900
    int tm_wday;    // day of week (Sunday = 0)
    int tm_yday;    // day of year (0 - 365)
    int tm_isdst;   // is summer time in effect?
    ...
};
*/

int main() {
    time_t now;
    time(&now);

    struct tm *tm = localtime(&now);
    printf("tm_year = %d\n", tm->tm_year);
    printf("tm_mon = %d\n", tm->tm_mon);
    printf("tm_wday = %d\n", tm->tm_wday);

    char str[100] = "insufficient space";
    strftime(str, sizeof(str), "%a %F %T", tm);
    printf("Current time: %s\n", str);
}