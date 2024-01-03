
#ifndef SOIL_UNSATURATION
#define SOIL_UNSATURATION

#include "Lookup_SoilLib.h"

void UnsaturatedWaterMove(
    double Water_input,
    double ET_o,
    double ET_u,
    double Es,
    double *Soil_Moisture_upper,
    double *Soil_Moisture_lower,
    double *SW_Infiltration,
    double *SW_Percolation_Upper,
    double *SW_Percolation_Lower,
    double SWV_gw,
    double SWV_rf,
    double *SW_Run_Infil,
    double *SW_Run_Satur,
    double Soil_thickness_upper,
    double Soil_thickness_lower,
    ST_SOIL_LIB_CELL *cell_soil_lib,
    int STEP_TIME
);

#endif