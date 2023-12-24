
#ifndef GEO2ASCII
#define GEO2ASCII

#include <stdlib.h>
#include <stdio.h>
#include "Constants.h"
#include "GEO_ST.h"

void Export_raster(
    int *rdata,
    FILE *fp_geo,
    int ncols,
    int nrows
);

void Export_header(
    FILE *fp_geo,
    ST_Header HD
);

void Export_GEO_data(
    int ncID,
    char varNAME[],
    ST_Header HD,
    char FP[]
);


#endif
