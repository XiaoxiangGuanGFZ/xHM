
#ifndef CHECK_DATA
#define CHECK_DATA

void Check_weather(
    int ncID_PRE, 
    int ncID_PRS, 
    int ncID_RHU, 
    int ncID_SSD, 
    int ncID_WIN, 
    int ncID_TEM_AVG, 
    int ncID_TEM_MAX, 
    int ncID_TEM_MIN,
    time_t START_TIME,
    time_t END_TIME
);

void Check_GEO(
    ncID_GEO
);

#endif

