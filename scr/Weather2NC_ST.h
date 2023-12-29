#ifndef WEATHER2NC_ST
#define WEATHER2NC_ST

#include "Constants.h"

typedef struct
{
    char FP_DATA[MAXCHAR];
    char FP_COOR[MAXCHAR];
    char FP_GEO[MAXCHAR];
    char FP_OUT[MAXCHAR];
    char VARNAME[MAXCHAR];
    int START_YEAR;
    int START_MONTH;
    int START_DAY;
    int START_HOUR;
    int STEP_TIME;
    int SITES;
} ST_weather_para;

typedef struct 
{
    int ID;
    double lon;
    double lat;
    int DEM;
} ST_COOR;


#endif