
#ifndef Evapotranspiration_ST
#define Evapotranspiration_ST

#include "constants.h"
typedef struct 
{
    char FP_GEO[MAXCHAR];
    char FP_PRE[MAXCHAR];
    char FP_WEATHER[MAXCHAR];
    int START_YEAR;
    int START_MONTH;
    int START_DAY;
    int START_HOUR;
    int STEP_TIME;
} GPara_ET_ST;


#endif



