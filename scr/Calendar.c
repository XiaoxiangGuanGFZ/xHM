
/*
 * SUMMARY:      Calendar.c
 * USAGE:        Calculate evaporation from soil (moisture)
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  
 * DESCRIP-END.
 * FUNCTIONS:    ET_soil()
 * 
 * COMMENTS:
 * 
 */

/***************************************************************
 * VARIABLEs:
 * int START_YEAR           - start year of the setup
 * int START_MONTH          - start month of the setup
 * int START_DAY            - start day of the setup
 * int START_HOUR           - start hour of the setup
 * int time_step            - time step of model setup, hours
 * int count                - number of steps since start point
 * struct tm *ptm           - struct tm
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "Calendar.h"

void tm_increment(
    int START_YEAR,
    int START_MONTH,
    int START_DAY,
    int START_HOUR,
    struct tm **ptm,
    int time_step,
    int count
)
{
    struct tm tm_start; 
    time_t t;
    tm_start.tm_hour = START_HOUR + 2;
    tm_start.tm_min = 0;
    tm_start.tm_sec = 0;
    tm_start.tm_mday = START_DAY;
    tm_start.tm_mon = START_MONTH - 1;
    tm_start.tm_year = START_YEAR - 1900;

    t = mktime(&tm_start);
    t = t + count * time_step * 3600;
    *ptm = gmtime(&t);
}

void main()
{
    
    char st[100];
    // time_t t, t2;
    // struct tm *ptm;
    // struct tm xmas = {0, 0, 1, 1, 0, 2000-1900};
    // t = mktime(&xmas);
    
    // strftime(st, 100, "%H:%M:%S %d-%m-%Y", &xmas);
    // printf("time: %s\n", st);
    // ptm = gmtime(&t);
    // strftime(st, 100, "%H:%M:%S %d-%m-%Y", ptm);
    // printf("time trans: %s\n", st);

    // for (size_t i = 0; i < 60; i++)
    // {
    //     t2 = t + i * 3600*24;
    //     ptm = gmtime(&t2);
    //     strftime(st, 100, "%H:%M:%S %d-%m-%Y", ptm);
    //     printf("time series: %s\n", st);
    // }
    struct tm *ptm2;
    tm_increment(
        2000, 1, 1, 0, &ptm2, 24, 1
    );
    strftime(st, 100, "%H:%M:%S %d-%m-%Y", ptm2);
    printf("test: %s\n", st);
}

