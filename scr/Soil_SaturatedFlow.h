
#ifndef SOIL_SATURATEDFLOW
#define SOIL_SATURATEDFLOW

#include "HM_ST.h"

double Soil_Satu_grad
(
    /*******
     *  \gamma_{i,j,k}
     * 
    */
    double Soil_Conduct_Sat,
    double Soil_Thickness,
    double n,
    double Cell_WT_z,
    double Cell_WT_k
);

void Soil_Satu_Outflow(
    double Cell_WT_z,
    int z_offset,
    int *neighbor,
    double *Cell_WT_rf,
    double *Cell_q,
    double *Qout,
    double Soil_Conduct_Sat,
    double Soil_Thickness,
    double b
);

void Initialize_Soil_Satur(
    CELL_VAR_SOIL *data_SOIL,
    int *data_DEM,
    int NODATA_value,
    int ncols,
    int nrows);


double Soil_Satu_Stream(
    double z,
    double stream_length,
    double stream_depth,
    double stream_width,
    double Soil_Conduct_Sat,
    double Soil_Thickness,
    double b
);


void Soil_Satu_Move(
    int *data_DEM,
    int *data_STR,
    int *data_SOILTYPE,
    CELL_VAR_STREAM *data_STREAM,
    CELL_VAR_SOIL *data_SOIL,
    ST_SoilLib *soillib,
    ST_SoilID *soilID,
    double Soil_Thickness,
    double Soil_d1,
    double Soil_d2,
    double stream_depth,
    double stream_width,
    int NODATA_value,
    int ncols,
    int nrows,
    double step_space,
    int step_time);


#endif