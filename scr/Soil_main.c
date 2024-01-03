


#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
// #include <netcdf.h>

#include "constants.h"
#include "GEO_ST.h"
#include "Lookup_SoilLib.h"
#include "Soil_Infiltration.h"
#include "Soil_Percolation.h"
#include "HM_ST.h"

void main(
    int argc, char *argv[]
)
{
    char FP_SOILLIB[] = "D:/xHM/example_data/SOIL_LIB.txt";
    ST_SoilLib soillib[13];
    Import_soillib(FP_SOILLIB, soillib);
    printf("%10s%20s%10s%10s%10s\n",
        "Code", "Texture", "Residual", "Porosity", "Bubbling"
    );

    char FP_SOILID[] = "D:/xHM/example_data/SOIL_HWSD_ID.txt";
    ST_SoilID soilID[1000];
    Import_soil_HWSD_ID(FP_SOILID, soilID);

    int soil_ID =  11375;
    ST_SOIL_LIB_CELL soil_cell_para;
    Lookup_Soil_CELL(soil_ID, &soil_cell_para, soillib, soilID);
    
    printf("%10d%20s%10.1f%10.1f%10.1f\n",
           (soil_cell_para.Topsoil)->Code,
           (soil_cell_para.Topsoil)->Texture,
           (soil_cell_para.Topsoil)->Residual,
           (soil_cell_para.Topsoil)->Porosity,
           (soil_cell_para.Topsoil)->Bubbling);

    /*****************************************/
    CELL_SOIL_VAR cell_soil;
    int STEP_TIME = 24;
    double Water_input = 0.0002; // m/h
    double Soil_Moisture = 0.5;
    cell_soil.SM_Upper = 0.5;
    cell_soil.SW_Infiltration = Soil_Infiltration(
        Water_input, cell_soil.SM_Upper,
        (soil_cell_para.Topsoil)->Porosity / 100,
        (soil_cell_para.Topsoil)->SatHydrauCond,
        (soil_cell_para.Topsoil)->AirEntryPresHead,
        (soil_cell_para.Topsoil)->PoreSizeDisP,
        STEP_TIME);
    printf("Infiltration [m]: %.5f\n", cell_soil.SW_Infiltration);
    cell_soil.SW_SR_Infil = Water_input * STEP_TIME - cell_soil.SW_Infiltration;

    double SThick_upper = 0.2;
    cell_soil.SW_Percolation_Upper = Percolation(
        cell_soil.SM_Upper,
        cell_soil.SW_Infiltration,
        SThick_upper,
        (soil_cell_para.Topsoil)->Porosity / 100,
        (soil_cell_para.Topsoil)->Residual / 100,
        (soil_cell_para.Topsoil)->SatHydrauCond,
        (soil_cell_para.Topsoil)->PoreSizeDisP,
        STEP_TIME);
    printf("Percolation upper [m]: %.5f", cell_soil.SW_Percolation_Upper);
}


