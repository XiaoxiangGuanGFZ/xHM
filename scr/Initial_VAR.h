
#ifndef INITIAL_VAR
#define INITIAL_VAR

#include "HM_ST.h"

void Initialize_RADIA(
    CELL_VAR_RADIA *st);

void Initialize_ET(
    CELL_VAR_ET *st);

void Initialize_SOIL(
    CELL_VAR_SOIL *st);

void Initialize_Soil_Satur(
    CELL_VAR_SOIL *data_SOIL,
    int *data_DEM,
    int NODATA_value,
    int ncols,
    int nrows);

#endif



