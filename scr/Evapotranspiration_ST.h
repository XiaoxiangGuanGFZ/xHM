
#ifndef Evapotranspiration_ST
#define Evapotranspiration_ST

#include "constants.h"

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
    double Albedo_o[12];
    double Albedo_u[12];
    double LAI_o[12];
    double LAI_u[12];
    double rs_min_o;
    double rs_min_u;    // Minimum stomatal resistance of vegetation
    double Rpc;
    double CAN_H;       // canopy height
    double CAN_RZ;      // canopy reference height
    double d_o[12];     // displacement height
    double d_u[12];
    double z0_o[12];    // roughness height
    double z0_u[12];
} ST_CELL_VEG;

#endif
