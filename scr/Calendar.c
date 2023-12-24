

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void main()
{
    time_t t, t2;
    struct tm *ptm;
    struct tm xmas = {0, 0, 1, 1, 0, 2000-1900};
    t = mktime(&xmas);
    char st[100];
    strftime(st, 100, "%H:%M:%S %d-%m-%Y", &xmas);
    printf("time: %s\n", st);
    ptm = gmtime(&t);
    strftime(st, 100, "%H:%M:%S %d-%m-%Y", ptm);
    printf("time trans: %s\n", st);

    for (size_t i = 0; i < 60; i++)
    {
        t2 = t + i * 3600*24;
        ptm = gmtime(&t2);
        strftime(st, 100, "%H:%M:%S %d-%m-%Y", ptm);
        printf("time series: %s\n", st);
    }
}