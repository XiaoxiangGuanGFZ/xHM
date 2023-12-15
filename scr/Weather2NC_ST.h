#ifndef WEATHER2NC_ST
#define WEATHER2NC_ST

#ifndef MAXCHAR
#define MAXCHAR 10000
#endif

typedef struct
{
    char FP_PRE[MAXCHAR];
    char FP_COOR[MAXCHAR];
    char FP_WEATHER[MAXCHAR];
    char FP_GEO[MAXCHAR];
    char FP_WEA_OUT[MAXCHAR];
    int START_YEAR;
    int START_MONTH;
    int START_DAY;
    int START_HOUR;
    int STEP;
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