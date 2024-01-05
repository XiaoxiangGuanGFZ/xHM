
/*
 * SUMMARY:      Check_DATA.c
 * USAGE:        check the attributes in weather variables NetCDF files
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Jan-2024
 * DESCRIPTION:  the investigated attributes: ncols, nrows, cellsize_m, STEP_TIME
 * 
 * DESCRIP-END.
 * FUNCTIONS:    Check_weather()
 * 
 * COMMENTS:
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <netcdf.h>
#include "Check_DATA.h"

void Check_weather(
    int ncID_PRE, 
    int ncID_PRS, 
    int ncID_RHU, 
    int ncID_SSD, 
    int ncID_WIN, 
    int ncID_TEM_AVG, 
    int ncID_TEM_MAX, 
    int ncID_TEM_MIN
)
{
    int attV_PRE, attV_PRS, attV_SSD, attV_RHU, attV_WIN, attV_TEM_AVG, attV_TEM_MAX, attV_TEM_MIN;
    // global attribute: ncols
    nc_get_att_int(ncID_PRE, NC_GLOBAL, "ncols", &attV_PRE);
    nc_get_att_int(ncID_PRS, NC_GLOBAL, "ncols", &attV_PRS);
    nc_get_att_int(ncID_RHU, NC_GLOBAL, "ncols", &attV_RHU);
    nc_get_att_int(ncID_SSD, NC_GLOBAL, "ncols", &attV_SSD);
    nc_get_att_int(ncID_WIN, NC_GLOBAL, "ncols", &attV_WIN);
    nc_get_att_int(ncID_TEM_AVG, NC_GLOBAL, "ncols", &attV_TEM_AVG);
    nc_get_att_int(ncID_TEM_MAX, NC_GLOBAL, "ncols", &attV_TEM_MAX);
    nc_get_att_int(ncID_TEM_MIN, NC_GLOBAL, "ncols", &attV_TEM_MIN);
    if (
        !(attV_PRE == attV_PRS &&
          attV_PRE == attV_RHU &&
          attV_PRE == attV_SSD &&
          attV_PRE == attV_WIN &&
          attV_PRE == attV_TEM_AVG &&
          attV_PRE == attV_TEM_MAX &&
          attV_PRE == attV_TEM_MIN))
    {
        printf("Weather data share different global attribute: ncols\n");
        exit(-2);
    }
    // global attribute: nrows
    nc_get_att_int(ncID_PRE, NC_GLOBAL, "nrows", &attV_PRE);
    nc_get_att_int(ncID_PRS, NC_GLOBAL, "nrows", &attV_PRS);
    nc_get_att_int(ncID_RHU, NC_GLOBAL, "nrows", &attV_RHU);
    nc_get_att_int(ncID_SSD, NC_GLOBAL, "nrows", &attV_SSD);
    nc_get_att_int(ncID_WIN, NC_GLOBAL, "nrows", &attV_WIN);
    nc_get_att_int(ncID_TEM_AVG, NC_GLOBAL, "nrows", &attV_TEM_AVG);
    nc_get_att_int(ncID_TEM_MAX, NC_GLOBAL, "nrows", &attV_TEM_MAX);
    nc_get_att_int(ncID_TEM_MIN, NC_GLOBAL, "nrows", &attV_TEM_MIN);
    if (
        !(attV_PRE == attV_PRS &&
          attV_PRE == attV_RHU &&
          attV_PRE == attV_SSD &&
          attV_PRE == attV_WIN &&
          attV_PRE == attV_TEM_AVG &&
          attV_PRE == attV_TEM_MAX &&
          attV_PRE == attV_TEM_MIN))
    {
        printf("Weather data share different global attribute: nrows\n");
        exit(-2);
    }

    // global attribute: cellsize_m
    nc_get_att_int(ncID_PRE, NC_GLOBAL, "cellsize_m", &attV_PRE);
    nc_get_att_int(ncID_PRS, NC_GLOBAL, "cellsize_m", &attV_PRS);
    nc_get_att_int(ncID_RHU, NC_GLOBAL, "cellsize_m", &attV_RHU);
    nc_get_att_int(ncID_SSD, NC_GLOBAL, "cellsize_m", &attV_SSD);
    nc_get_att_int(ncID_WIN, NC_GLOBAL, "cellsize_m", &attV_WIN);
    nc_get_att_int(ncID_TEM_AVG, NC_GLOBAL, "cellsize_m", &attV_TEM_AVG);
    nc_get_att_int(ncID_TEM_MAX, NC_GLOBAL, "cellsize_m", &attV_TEM_MAX);
    nc_get_att_int(ncID_TEM_MIN, NC_GLOBAL, "cellsize_m", &attV_TEM_MIN);
    if (
        !(attV_PRE == attV_PRS &&
          attV_PRE == attV_RHU &&
          attV_PRE == attV_SSD &&
          attV_PRE == attV_WIN &&
          attV_PRE == attV_TEM_AVG &&
          attV_PRE == attV_TEM_MAX &&
          attV_PRE == attV_TEM_MIN))
    {
        printf("Weather data share different global attribute: cellsize_m\n");
        exit(-2);
    }
    // STEP_TIME
    int varID;
    int attID;
    nc_inq_varid(ncID_PRE, "PRE", &varID); nc_get_att_int(ncID_PRE, varID, "STEP_TIME", &attV_PRE);
    nc_inq_varid(ncID_PRS, "PRS", &varID); nc_get_att_int(ncID_PRS, varID, "STEP_TIME", &attV_PRS);
    nc_inq_varid(ncID_SSD, "SSD", &varID); nc_get_att_int(ncID_SSD, varID, "STEP_TIME", &attV_SSD);
    nc_inq_varid(ncID_RHU, "RHU", &varID); nc_get_att_int(ncID_RHU, varID, "STEP_TIME", &attV_RHU);
    nc_inq_varid(ncID_WIN, "WIN", &varID); nc_get_att_int(ncID_WIN, varID, "STEP_TIME", &attV_WIN);
    nc_inq_varid(ncID_TEM_AVG, "TEM_AVG", &varID); nc_get_att_int(ncID_TEM_AVG, varID, "STEP_TIME", &attV_TEM_AVG);
    nc_inq_varid(ncID_TEM_MAX, "TEM_MAX", &varID); nc_get_att_int(ncID_TEM_MAX, varID, "STEP_TIME", &attV_TEM_MAX);
    nc_inq_varid(ncID_TEM_MIN, "TEM_MIN", &varID); nc_get_att_int(ncID_TEM_MIN, varID, "STEP_TIME", &attV_TEM_MIN);

    if (
        !(attV_PRE == attV_PRS &&
          attV_PRE == attV_RHU &&
          attV_PRE == attV_SSD &&
          attV_PRE == attV_WIN &&
          attV_PRE == attV_TEM_AVG &&
          attV_PRE == attV_TEM_MAX &&
          attV_PRE == attV_TEM_MIN))
    {
        printf("Weather data share different variable attribute: STEP_TIME\n");
        exit(-2);
    }
}


