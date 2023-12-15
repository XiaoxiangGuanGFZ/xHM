#ifndef WEATHER2NC
#define WEATHER2NC

#include "Weather2NC_ST.h"

void Import_weather_para(
    char FP[],
    ST_weather_para *GP,
    int display);


void Import_COOR(
    char FP_COOR[],
    ST_COOR *coor);

void Import_weather_data(
    char FP[],
    int *rdata,
    int SITES,
    int *nrows
);


#endif


