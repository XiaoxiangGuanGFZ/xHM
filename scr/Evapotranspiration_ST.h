
#ifndef Evapotranspiration_ST
#define Evapotranspiration_ST

#include "constants.h"

// coefficient in terrestrial radiation computation
#define as 0.25
#define bs 0.5

// shortwave reflection coefficient of soil/ground
#define ALBEDO_SOIL 0.159

// rs_max: maximum (cuticular) resistance [50 s/cm = 5000s/m]
#define rs_max 5000.0

typedef struct 
{
    char FP_GEO[MAXCHAR];
    char FP_PRE[MAXCHAR];
    char FP_WEATHER[MAXCHAR];
    char FP_VEGLIB[MAXCHAR];
    int START_YEAR;
    int START_MONTH;
    int START_DAY;
    int START_HOUR;
    int STEP_TIME;
} GPara_ET_ST;

typedef struct 
{
    double PRS;
    double RHU;
    double SSD;
    double WIN;
    double TEM_AVG;
    double TEM_MAX;
    double TEM_MIN;
} ST_Weather;

typedef struct
{
    int CLASS;
    double CAN_FRAC;
    int Understory;
    double Albedo_o;
    double Albedo_u;
    double LAI_o;
    double LAI_u;
    double rs_min_o;
    double rs_min_u;
    double Rpc;
    double CAN_H;
    double CAN_RZ;
    double d_o;
    double d_u;
    double z0_o;
    double z0_u;
} ST_CELL_VEG;

#endif



