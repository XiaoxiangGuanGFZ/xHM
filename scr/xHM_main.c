/*
 * SUMMARY:      xHM_main.c
 * USAGE:        hydrological modeling module
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Jan-2024
 * DESCRIPTION:  simulate hydrological processes:
 *
 * DESCRIP-END.
 * FUNCTIONS:
 *
 * COMMENTS:
 *
 *
 * References:
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <netcdf.h>
#include <time.h>
#include "constants.h"
#include "Calendar.h"
#include "HM_ST.h"
#include "HM_GlobalPara.h"
#include "OutNamelist.h"
#include "NC_copy_global_att.h"
#include "Check_Data.h"
#include "Evapotranspiration_ST.h"
#include "Evapotranspiration.h"
#include "Evapotranspiration_Energy.h"
#include "Evaporation_soil.h"
#include "Soil_Desorption.h"
#include "GEO_ST.h"
#include "Resistance.h"
#include "Lookup_VegLib.h"
#include "Lookup_SoilLib.h"
#include "Initial_VAR.h"
#include "NetCDF_IO_geo.h"
#include "UH_Generation.h"
#include "UH_Routing.h"
#include "Soil_UnsaturatedMove.h"
#include "Soil_SaturatedFlow.h"
#include "Route_Channel.h"
#include "Route_Outlet.h"

void malloc_error(
    int *data);

char *DateString(
    time_t *tm
);

int main(int argc, char *argv[])
{
    time_t tm;  //datatype from <time.h>
    
    /******************************************************************************
     *                        read global parameter file
     ******************************************************************************/
    time(&tm); printf("--------- %s read global parameter: ", DateString(&tm));
    GLOBAL_PARA GP;
    Initialize_GlobalPara(&GP);
    Import_GlobalPara(*(++argv), &GP); printf("Done! \n");
    Print_GlobalPara(&GP); // print the field-value pairs to screen
    double ws_obs_z;       /* the measurement height of wind speed, [m] */
    ws_obs_z = GP.WIN_H;
    time(&tm); printf("--------- %s read outnamelist: ", DateString(&tm));
    char WS_OUT[MAXCHAR];
    strcpy(WS_OUT, GP.PATH_OUT);
    OUT_NAME_LIST outnl;
    OUT_NAME_LIST outnl_ncid;
    Initialize_Outnamelist(&outnl);
    Import_Outnamelist(GP.FP_OUTNAMELIST, &outnl); printf("Done! \n");
    /*****************************************************************************
     *                          model simulation period
     ******************************************************************************/
    int time_steps_run; // number of simulation steps
    time_t start_time, end_time;
    struct tm tm_start, tm_end;
    tm_start.tm_hour = GP.START_HOUR + 1;
    tm_end.tm_hour = GP.END_HOUR + 1;
    tm_start.tm_min = 0;
    tm_end.tm_min = 0;
    tm_start.tm_sec = 0;
    tm_end.tm_sec = 0;
    tm_start.tm_mday = GP.START_DAY;
    tm_end.tm_mday = GP.END_DAY;
    tm_start.tm_mon = GP.START_MONTH - 1;
    tm_end.tm_mon = GP.END_MONTH - 1;
    tm_start.tm_year = GP.START_YEAR - 1900;
    tm_end.tm_year = GP.END_YEAR - 1900;
    // Daylight Saving Time not in effect;Specify UTC time zone
    tm_start.tm_isdst = 0;
    tm_end.tm_isdst = 0;
    start_time = mktime(&tm_start);
    end_time = mktime(&tm_end);
    if (start_time > end_time)
    {
        printf("Error: invalid start time or end time for the model running. Please check the global parameter file!\n");
        exit(-2);
    }

    time_steps_run = (end_time - start_time) / (3600 * GP.STEP_TIME) + 1;
    // printf("time_steps_run: %d\n", time_steps_run);
    /******************************************************************************
     *                              read GEO info
     ******************************************************************************/
    time(&tm); printf("--------- %s read GEO data: \n", DateString(&tm));
    int status_nc;
    int ncID_GEO;
    ST_Header GEO_header;
    int cellsize_m;  // unit: m
    int cellarea_m;  // unit: m2
    nc_open(GP.FP_GEO, NC_NOWRITE, &ncID_GEO);
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "ncols", &GEO_header.ncols);
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "nrows", &GEO_header.nrows);
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "cellsize_m", &cellsize_m);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "xllcorner", &GEO_header.xllcorner);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "yllcorner", &GEO_header.yllcorner);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "cellsize", &GEO_header.cellsize);
    cellarea_m = cellsize_m * cellsize_m;

    int cell_counts_total;
    cell_counts_total = GEO_header.ncols * GEO_header.nrows;
    printf("* cell_counts_total: %d\n", cell_counts_total);
    printf("* ncols: %d\n* nrows: %d\n* xllcorner: %.12f\n* yllcorner: %.12f\n* cellsize: %.12f\n",
           GEO_header.ncols, GEO_header.nrows, GEO_header.xllcorner, GEO_header.yllcorner, GEO_header.cellsize);
    printf("* cellsize (m): %.1f\n", (double) cellsize_m);
    int varID_VEGTYPE, varID_VEGFRAC, varID_lon, varID_lat, varID_SOILTYPE, varID_STR, varID_DEM, varID_FDR;
    int *data_VEGTYPE, *data_VEGFRAC, *data_STR, *data_DEM, *data_SOILTYPE, *data_FDR;
    double *data_lon, *data_lat;

    data_VEGTYPE = (int *)malloc(sizeof(int) * cell_counts_total);
    data_VEGFRAC = (int *)malloc(sizeof(int) * cell_counts_total);
    data_SOILTYPE = (int *)malloc(sizeof(int) * cell_counts_total);
    data_DEM = (int *)malloc(sizeof(int) * cell_counts_total);
    data_STR = (int *)malloc(sizeof(int) * cell_counts_total);
    data_FDR = (int *)malloc(sizeof(int) * cell_counts_total);
    data_lat = (double *)malloc(sizeof(double) * GEO_header.nrows);
    data_lon = (double *)malloc(sizeof(double) * GEO_header.ncols);
    
    nc_inq_varid(ncID_GEO, "VEGTYPE", &varID_VEGTYPE);
    nc_inq_varid(ncID_GEO, "VEGFRAC", &varID_VEGFRAC);
    nc_inq_varid(ncID_GEO, "SOILTYPE", &varID_SOILTYPE);
    nc_inq_varid(ncID_GEO, "DEM", &varID_DEM);
    nc_inq_varid(ncID_GEO, "STR", &varID_STR);
    nc_inq_varid(ncID_GEO, "FDR", &varID_FDR);
    nc_inq_varid(ncID_GEO, "lon", &varID_lon);
    nc_inq_varid(ncID_GEO, "lat", &varID_lat);

    nc_get_var_int(ncID_GEO, varID_VEGTYPE, data_VEGTYPE);
    nc_get_var_int(ncID_GEO, varID_VEGFRAC, data_VEGFRAC);
    nc_get_var_int(ncID_GEO, varID_SOILTYPE, data_SOILTYPE);
    nc_get_var_int(ncID_GEO, varID_DEM, data_DEM);
    nc_get_var_int(ncID_GEO, varID_STR, data_STR);
    nc_get_var_int(ncID_GEO, varID_FDR, data_FDR);
    nc_get_var_double(ncID_GEO, varID_lon, data_lon);
    nc_get_var_double(ncID_GEO, varID_lat, data_lat);

    Check_GEO(ncID_GEO);   // check the GEO data
    time(&tm); printf("--------- %s read GEO data: ", DateString(&tm)); printf("Done! \n");

    /******************************************************************************
     *                      read soil and vegetation library
     ******************************************************************************/
    time(&tm); printf("--------- %s read vegetation and soil library: ", DateString(&tm));
    // import the vegetation library
    ST_VegLib veglib[11];
    Import_veglib(GP.FP_VEGLIB, veglib);
    // import the soil property library abd HWSD IDs
    ST_SoilLib soillib[13];
    Import_soillib(GP.FP_SOILLIB, soillib);
    ST_SoilID soilID[1000];
    Import_soil_HWSD_ID(GP.FP_SOIL_HWSD_ID, soilID);
    printf("Done! \n");

    /******************************************************************************
     *                      read gridded weather data
     ******************************************************************************/
    time(&tm); printf("--------- %s read weather forcing: \n", DateString(&tm));
    int ncID_PRE, ncID_PRS, ncID_RHU, ncID_SSD, ncID_WIN, ncID_TEM_AVG, ncID_TEM_MAX, ncID_TEM_MIN;
    int varID_PRE, varID_PRS, varID_RHU, varID_SSD, varID_WIN, varID_TEM_AVG, varID_TEM_MAX, varID_TEM_MIN;
    int *data_PRE;
    int *data_PRS;
    int *data_SSD;
    int *data_RHU;
    int *data_WIN;
    int *data_TEM_AVG;
    int *data_TEM_MAX;
    int *data_TEM_MIN;

    // open the nc files
    status_nc = nc_open(GP.FP_PRE, NC_NOWRITE, &ncID_PRE);
    handle_error(status_nc, GP.FP_PRE);
    status_nc = nc_open(GP.FP_PRS, NC_NOWRITE, &ncID_PRS);
    handle_error(status_nc, GP.FP_PRS);
    status_nc = nc_open(GP.FP_RHU, NC_NOWRITE, &ncID_RHU);
    handle_error(status_nc, GP.FP_RHU);
    status_nc = nc_open(GP.FP_SSD, NC_NOWRITE, &ncID_SSD);
    handle_error(status_nc, GP.FP_SSD);
    status_nc = nc_open(GP.FP_WIN, NC_NOWRITE, &ncID_WIN);
    handle_error(status_nc, GP.FP_WIN);
    status_nc = nc_open(GP.FP_TEM_AVG, NC_NOWRITE, &ncID_TEM_AVG);
    handle_error(status_nc, GP.FP_TEM_AVG);
    status_nc = nc_open(GP.FP_TEM_MAX, NC_NOWRITE, &ncID_TEM_MAX);
    handle_error(status_nc, GP.FP_TEM_MAX);
    status_nc = nc_open(GP.FP_TEM_MIN, NC_NOWRITE, &ncID_TEM_MIN);
    handle_error(status_nc, GP.FP_TEM_MIN);
    /* check the weather datasets: compatibility, consistency */
    Check_weather(ncID_PRE, ncID_PRS, ncID_RHU, ncID_SSD, ncID_WIN, ncID_TEM_AVG, ncID_TEM_MAX, ncID_TEM_MIN, start_time, end_time, GP.STEP_TIME);

    int dimID_time;
    int varID_time;
    // the starting time of the variable series from NC files
    long t_PRE, t_PRS, t_SSD, t_RHU, t_WIN, t_TEM_AVG, t_TEM_MAX, t_TEM_MIN;
    // the length (steps) of the variable series from NC files
    int time_steps_PRE, time_steps_PRS, time_steps_RHU, time_steps_SSD, time_steps_WIN, time_steps_TEM_AVG, time_steps_TEM_MAX, time_steps_TEM_MIN;

    size_t index = 0;

    nc_inq_varid(ncID_PRE, "time", &varID_time);
    nc_get_var1_long(ncID_PRE, varID_time, &index, &t_PRE); // the first value in the time series
    nc_inq_dimid(ncID_PRE, "time", &dimID_time);
    nc_inq_dimlen(ncID_PRE, dimID_time, &time_steps_PRE);   // the length of the time dimension

    nc_inq_varid(ncID_PRS, "time", &varID_time);
    nc_get_var1_long(ncID_PRS, varID_time, &index, &t_PRS);
    nc_inq_dimid(ncID_PRS, "time", &dimID_time);
    nc_inq_dimlen(ncID_PRS, dimID_time, &time_steps_PRS);

    nc_inq_varid(ncID_SSD, "time", &varID_time);
    nc_get_var1_long(ncID_SSD, varID_time, &index, &t_SSD);
    nc_inq_dimid(ncID_SSD, "time", &dimID_time);
    nc_inq_dimlen(ncID_SSD, dimID_time, &time_steps_SSD);

    nc_inq_varid(ncID_RHU, "time", &varID_time);
    nc_get_var1_long(ncID_RHU, varID_time, &index, &t_RHU);
    nc_inq_dimid(ncID_RHU, "time", &dimID_time);
    nc_inq_dimlen(ncID_RHU, dimID_time, &time_steps_RHU);

    nc_inq_varid(ncID_WIN, "time", &varID_time);
    nc_get_var1_long(ncID_WIN, varID_time, &index, &t_WIN);
    nc_inq_dimid(ncID_WIN, "time", &dimID_time);
    nc_inq_dimlen(ncID_WIN, dimID_time, &time_steps_WIN);

    nc_inq_varid(ncID_TEM_AVG, "time", &varID_time);
    nc_get_var1_long(ncID_TEM_AVG, varID_time, &index, &t_TEM_AVG);
    nc_inq_dimid(ncID_TEM_AVG, "time", &dimID_time);
    nc_inq_dimlen(ncID_TEM_AVG, dimID_time, &time_steps_TEM_AVG);

    nc_inq_varid(ncID_TEM_MAX, "time", &varID_time);
    nc_get_var1_long(ncID_TEM_MAX, varID_time, &index, &t_TEM_MAX);
    nc_inq_dimid(ncID_TEM_MAX, "time", &dimID_time);
    nc_inq_dimlen(ncID_TEM_MAX, dimID_time, &time_steps_TEM_MAX);

    nc_inq_varid(ncID_TEM_MIN, "time", &varID_time);
    nc_get_var1_long(ncID_TEM_MIN, varID_time, &index, &t_TEM_MIN);
    nc_inq_dimid(ncID_TEM_MIN, "time", &dimID_time);
    nc_inq_dimlen(ncID_TEM_MIN, dimID_time, &time_steps_TEM_MIN);

    // print (preview) the info table of weather forcing NC datasets
    time_t tm_buf;
    printf("* Info table of the forcing datasets:\n");
    printf("* %10s%25s%10s%10s\n", "forcing", "start_date", "length", "size(GB)");
    tm_buf = t_PRE - 3600; printf("* %10s%25s%10d%10.3f\n", "PRE", DateString(&tm_buf), time_steps_PRE, (float)sizeof(int) * time_steps_PRE * cell_counts_total / 1024 / 1024 / 1024);
    tm_buf = t_PRS - 3600; printf("* %10s%25s%10d%10.3f\n", "PRS", DateString(&tm_buf), time_steps_PRS, (float)sizeof(int) * time_steps_PRS * cell_counts_total / 1024 / 1024 / 1024);
    tm_buf = t_RHU - 3600; printf("* %10s%25s%10d%10.3f\n", "RHU", DateString(&tm_buf), time_steps_RHU, (float)sizeof(int) * time_steps_RHU * cell_counts_total / 1024 / 1024 / 1024);
    tm_buf = t_SSD - 3600; printf("* %10s%25s%10d%10.3f\n", "SSD", DateString(&tm_buf), time_steps_SSD, (float)sizeof(int) * time_steps_SSD * cell_counts_total / 1024 / 1024 / 1024);
    tm_buf = t_WIN - 3600; printf("* %10s%25s%10d%10.3f\n", "WIN", DateString(&tm_buf), time_steps_WIN, (float)sizeof(int) * time_steps_WIN * cell_counts_total / 1024 / 1024 / 1024);
    tm_buf = t_TEM_AVG - 3600; printf("* %10s%25s%10d%10.3f\n", "TEM_AVG", DateString(&tm_buf), time_steps_TEM_AVG, (float)sizeof(int) * time_steps_TEM_AVG * cell_counts_total / 1024 / 1024 / 1024);
    tm_buf = t_TEM_MAX - 3600; printf("* %10s%25s%10d%10.3f\n", "TEM_MAX", DateString(&tm_buf), time_steps_TEM_MAX, (float)sizeof(int) * time_steps_TEM_MAX * cell_counts_total / 1024 / 1024 / 1024);
    tm_buf = t_TEM_MIN - 3600; printf("* %10s%25s%10d%10.3f\n", "TEM_MIN", DateString(&tm_buf), time_steps_TEM_MIN, (float)sizeof(int) * time_steps_TEM_MIN * cell_counts_total / 1024 / 1024 / 1024);

    // time interval (offset) between model-simulation starting date and forcing data starting date
    int t_offset_PRE, t_offset_PRS, t_offset_SSD, t_offset_RHU, t_offset_WIN;
    int t_offset_TEM_AVG, t_offset_TEM_MAX, t_offset_TEM_MIN;
    t_offset_PRE = (start_time - t_PRE) / (GP.STEP_TIME * 3600);
    t_offset_PRS = (start_time - t_PRS) / (GP.STEP_TIME * 3600);
    t_offset_SSD = (start_time - t_SSD) / (GP.STEP_TIME * 3600);
    t_offset_RHU = (start_time - t_RHU) / (GP.STEP_TIME * 3600);
    t_offset_WIN = (start_time - t_WIN) / (GP.STEP_TIME * 3600);
    t_offset_TEM_AVG = (start_time - t_TEM_AVG) / (GP.STEP_TIME * 3600);
    t_offset_TEM_MAX = (start_time - t_TEM_MAX) / (GP.STEP_TIME * 3600);
    t_offset_TEM_MIN = (start_time - t_TEM_MIN) / (GP.STEP_TIME * 3600);

    // get the ids of the variables 
    status_nc = nc_inq_varid(ncID_PRE, "PRE", &varID_PRE);
    handle_error(status_nc, GP.FP_PRE);
    status_nc = nc_inq_varid(ncID_PRS, "PRS", &varID_PRS);
    handle_error(status_nc, GP.FP_PRS);
    status_nc = nc_inq_varid(ncID_SSD, "SSD", &varID_SSD);
    handle_error(status_nc, GP.FP_SSD);
    status_nc = nc_inq_varid(ncID_RHU, "RHU", &varID_RHU);
    handle_error(status_nc, GP.FP_RHU);
    status_nc = nc_inq_varid(ncID_WIN, "WIN", &varID_WIN);
    handle_error(status_nc, GP.FP_WIN);
    status_nc = nc_inq_varid(ncID_TEM_AVG, "TEM_AVG", &varID_TEM_AVG);
    handle_error(status_nc, GP.FP_TEM_AVG);
    status_nc = nc_inq_varid(ncID_TEM_MAX, "TEM_MAX", &varID_TEM_MAX);
    handle_error(status_nc, GP.FP_TEM_MAX);
    status_nc = nc_inq_varid(ncID_TEM_MIN, "TEM_MIN", &varID_TEM_MIN);
    handle_error(status_nc, GP.FP_TEM_MIN);
    
    // allocate memory for only one-step data (2D raster)
    data_PRE = (int *)malloc(sizeof(int) * cell_counts_total);
    malloc_error(data_PRE);
    data_PRS = (int *)malloc(sizeof(int) * cell_counts_total);
    malloc_error(data_PRS);
    data_SSD = (int *)malloc(sizeof(int) * cell_counts_total);
    malloc_error(data_SSD);
    data_RHU = (int *)malloc(sizeof(int) * cell_counts_total);
    malloc_error(data_RHU);
    data_WIN = (int *)malloc(sizeof(int) * cell_counts_total);
    malloc_error(data_WIN);
    data_TEM_AVG = (int *)malloc(sizeof(int) * cell_counts_total);
    malloc_error(data_TEM_AVG);
    data_TEM_MAX = (int *)malloc(sizeof(int) * cell_counts_total);
    malloc_error(data_TEM_MAX);
    data_TEM_MIN = (int *)malloc(sizeof(int) * cell_counts_total);
    malloc_error(data_TEM_MIN);

    // read relevant (corresponding simulation period) weather forcing into memory
    // nc_get_vara_*: extract variable ARRAY from nc dataset
    int nc_start[3] = {0, 0, 0};
    int nc_count[3] = {0, 0, 0};
    nc_count[0] = 1;
    nc_count[1] = GEO_header.nrows;
    nc_count[2] = GEO_header.ncols;
    
    nc_get_att_int(ncID_PRE, varID_PRE, "NODATA_value", &GEO_header.NODATA_value);
    // the scale_factor and offset parameters for NC variables
    double scale_PRE, scale_PRS, scale_SSD, scale_RHU, scale_WIN, scale_TEM_AVG, scale_TEM_MAX, scale_TEM_MIN;
    status_nc = nc_get_att_double(ncID_PRE, varID_PRE, "scale_factor", &scale_PRE);
    handle_error(status_nc, GP.FP_PRE);
    status_nc = nc_get_att_double(ncID_PRS, varID_PRS, "scale_factor", &scale_PRS);
    handle_error(status_nc, GP.FP_PRS);
    status_nc = nc_get_att_double(ncID_SSD, varID_SSD, "scale_factor", &scale_SSD);
    handle_error(status_nc, GP.FP_SSD);
    status_nc = nc_get_att_double(ncID_RHU, varID_RHU, "scale_factor", &scale_RHU);
    handle_error(status_nc, GP.FP_RHU);
    status_nc = nc_get_att_double(ncID_WIN, varID_WIN, "scale_factor", &scale_WIN);
    handle_error(status_nc, GP.FP_WIN);
    status_nc = nc_get_att_double(ncID_TEM_AVG, varID_TEM_AVG, "scale_factor", &scale_TEM_AVG);
    handle_error(status_nc, GP.FP_TEM_AVG);
    status_nc = nc_get_att_double(ncID_TEM_MAX, varID_TEM_MAX, "scale_factor", &scale_TEM_MAX);
    handle_error(status_nc, GP.FP_TEM_MAX);
    status_nc = nc_get_att_double(ncID_TEM_MIN, varID_TEM_MIN, "scale_factor", &scale_TEM_MIN);
    handle_error(status_nc, GP.FP_TEM_MIN);
    // printf("check scale_factor:\n");
    // printf("PRE: %.1f\n", scale_PRE);
    // printf("RHU: %.1f\n", scale_RHU);
    // printf("TEM_AVG: %.1f\n", scale_TEM_AVG);

    time(&tm); printf("--------- %s read weather forcing: Done!\n", DateString(&tm));
    /***********************************************************************************
     *                          set model running period
     ************************************************************************************/
    time_t run_time;
    run_time = start_time;
    int index_geo;
    int index_run;

    /***********************************************************************************
     *                      surface runoff routing - UH
     ************************************************************************************/
    time(&tm); printf("--------- %s prepare UH: \n", DateString(&tm));
    UH_Generation(GP.FP_GEO, GP.FP_UH, GP.STEP_TIME,
                  GP.Velocity_avg, GP.Velocity_max, GP.Velocity_min, GP.b, GP.c);

    int ncID_UH;
    nc_open(GP.FP_UH, NC_NOWRITE, &ncID_UH);
    int varID_UH[MAX_OUTLETS];
    int outlet_count;
    int outlet_index_row[MAX_OUTLETS];
    int outlet_index_col[MAX_OUTLETS];
    int UH_steps[MAX_OUTLETS];
    int UH_steps_total = 0;
    UH_Read(
        ncID_UH,
        varID_UH,
        &outlet_count,
        outlet_index_row,
        outlet_index_col,
        UH_steps);
    printf("* outlet_count: %d\n", outlet_count);
    printf("* %6s%6s%6s%6s\n", "outlet", "row", "col", "steps");
    for (size_t s = 0; s < outlet_count; s++)
    {
        printf("* %6d%6d%6d%6d\n", s, outlet_index_row[s], outlet_index_col[s], UH_steps[s]);
        UH_steps_total += UH_steps[s];
    }
    // printf("* UH_steps_total: %d\n", UH_steps_total);
    double *data_UH;
    data_UH = (double *)malloc(sizeof(double) * cell_counts_total * UH_steps_total);
    UH_Import(
        ncID_UH,
        varID_UH,
        outlet_count,
        cell_counts_total,
        UH_steps,
        &data_UH);
    time(&tm); printf("--------- %s prepare UH: ", DateString(&tm)); printf("Done!\n");
    /***********************************************************************************
     *              define and initialize the intermediate variables
     ***********************************************************************************/
    time(&tm); printf("--------- %s initialize intermediate data structures: \n", DateString(&tm));
    CELL_VAR_RADIA *data_RADIA;
    data_RADIA = (CELL_VAR_RADIA *)malloc(sizeof(CELL_VAR_RADIA) * cell_counts_total);
    
    CELL_VAR_ET *data_ET;
    data_ET = (CELL_VAR_ET *)malloc(sizeof(CELL_VAR_ET) * cell_counts_total);
    
    CELL_VAR_SOIL *data_SOIL;
    data_SOIL = (CELL_VAR_SOIL *)malloc(sizeof(CELL_VAR_SOIL) * cell_counts_total);
    
    CELL_VAR_STREAM *data_STREAM;
    data_STREAM = (CELL_VAR_STREAM *)malloc(sizeof(CELL_VAR_STREAM) * cell_counts_total);
    
    for (size_t i = 0; i < GEO_header.nrows; i++)
    {
        for (size_t j = 0; j < GEO_header.ncols; j++)
        {
            index_geo = i * GEO_header.ncols + j;
            Initialize_RADIA(data_RADIA + index_geo);
            Initialize_ET(data_ET + index_geo);
            Initialize_SOIL(data_SOIL + index_geo);
        }
    }
    printf("* data_RADIA\n");
    printf("* data_ET\n");

    Initialize_Soil_Satur(
        data_SOIL,
        data_DEM,
        GEO_header.NODATA_value,
        GEO_header.ncols,
        GEO_header.nrows);
    printf("* data_SOIL\n");

    Initialize_STREAM(
        &data_STREAM,
        data_STR,
        data_FDR,
        GP.ROUTE_CHANNEL_k,
        GEO_header.NODATA_value,
        GEO_header.ncols,
        GEO_header.nrows);
    printf("* data_STREAM\n");

    time(&tm); printf("--------- %s initialize intermediate data structures: ", DateString(&tm)); printf("Done!\n");
    /***********************************************************************************
     *              define the output variables (results) from simulation
     ***********************************************************************************/
    time(&tm); printf("--------- %s allocate memory for output variables: ", DateString(&tm));
    // char FP_OUT_VAR[MAXCHAR] = "";
    int *out_Rs, *out_L_sky, *out_Rno, *out_Rnu;
    int *out_Ep, *out_EI_o, *out_EI_u, *out_ET_o, *out_ET_u, *out_ET_s;
    int *out_Interception_o, *out_Interception_u, *out_Prec_net;
    int *out_SM_Upper, *out_SM_Lower, *out_SW_Percolation_Upper, *out_SW_Percolation_Lower;
    int *out_SW_Infiltration, *out_SW_Run_Infil, *out_SW_Run_Satur;
    int *out_SW_SUB_Qin, *out_SW_SUB_Qout, *out_SW_SUB_z, *out_SW_SUB_rise_upper, *out_SW_SUB_rise_lower, *out_SW_SUB_rf;
    int *out_SW_SUB_Qc, *out_Q_Channel;
    malloc_Outnamelist(
        outnl, &outnl_ncid, 
        cell_counts_total, time_steps_run, 
        &data_DEM, GEO_header, GP,
        &out_Rs, &out_L_sky, &out_Rno, &out_Rnu,
        &out_Ep, &out_EI_o, &out_EI_u, &out_ET_o, &out_ET_u, &out_ET_s,
        &out_Interception_o, &out_Interception_u, &out_Prec_net,
        &out_SM_Upper, &out_SM_Lower, &out_SW_Percolation_Upper, &out_SW_Percolation_Lower,
        &out_SW_Infiltration, 
        &out_SW_SUB_Qin, &out_SW_SUB_Qout, &out_SW_SUB_z, 
        &out_SW_SUB_rise_upper, &out_SW_SUB_rise_lower, 
        &out_SW_SUB_rf, &out_SW_SUB_Qc, &out_Q_Channel);
    out_SW_Run_Infil = (int *)malloc(sizeof(int) * time_steps_run * cell_counts_total); malloc_error(out_SW_Run_Infil);
    out_SW_Run_Satur = (int *)malloc(sizeof(int) * time_steps_run * cell_counts_total); malloc_error(out_SW_Run_Satur);

    double *Qout_SF_Infil, *Qout_SF_Satur, *Qout_Sub, *Qout_outlet;
    Qout_SF_Infil = (double *)malloc(sizeof(double) * outlet_count * time_steps_run);
    Qout_SF_Satur = (double *)malloc(sizeof(double) * outlet_count * time_steps_run);
    Qout_Sub = (double *)malloc(sizeof(double) * outlet_count * time_steps_run);
    Qout_outlet = (double *)malloc(sizeof(double) * outlet_count * time_steps_run);
    printf("Done!\n");
    /***********************************************************************************
     *                       define the iteration variables
     ***********************************************************************************/
    int t = 0;
    double cell_PRE, cell_WIN, cell_SSD, cell_RHU, cell_PRS, cell_TEM_AVG, cell_TEM_MAX, cell_TEM_MIN;

    ST_CELL_VEG cell_veg;
    ST_SOIL_LIB_CELL cell_soil;
    int cell_VEG_class;
    int cell_SOIL_ID;
    double cell_lat;
    int year;
    int month;
    int day;
    struct tm *tm_run;
    double Soil_Fe = 0.0;

    /***********************************************************************************
     *                     hydrological parameters
    ************************************************************************************/
    double Soil_Thickness, Soil_d1, Soil_d2, stream_depth, stream_width;
    Soil_Thickness = GP.SOIL_D;
    Soil_d1 = GP.SOIL_d1;
    Soil_d2 = GP.SOIL_d2;
    stream_depth = GP.STREAM_D;
    stream_width = GP.STREAM_W;
    /***********************************************************************************
     *                       xHM model iteration
     ***********************************************************************************/
    time(&tm); printf("--------- %s xHM hydrological processes simulating: ", DateString(&tm));
    while (run_time <= end_time)
    {
        tm_run = gmtime(&run_time);
        year = tm_run->tm_year + 1900;
        month = tm_run->tm_mon + 1;
        day = tm_run->tm_mday;
        // printf("%d-%02d-%02d\n", year, month, day);
        if (t % 730 == 0)
        {
            printf("*");
        }
        /***************
         * in each iteration of model simulation,
         * a map (time step) of forcing data is extracted into memory
         * for process simulation 
        */
        nc_start[0] = t_offset_PRE + t;
        status_nc = nc_get_vara_int(ncID_PRE, varID_PRE, nc_start, nc_count, data_PRE); // status_nc = nc_get_var_int(ncID_PRE, varID_PRE, data_PRE);
        handle_error(status_nc, GP.FP_PRE);

        nc_start[0] = t_offset_PRS + t;
        status_nc = nc_get_vara_int(ncID_PRS, varID_PRS, nc_start, nc_count, data_PRS);
        handle_error(status_nc, GP.FP_PRS);

        nc_start[0] = t_offset_SSD + t;
        status_nc = nc_get_vara_int(ncID_SSD, varID_SSD, nc_start, nc_count, data_SSD);
        handle_error(status_nc, GP.FP_SSD);

        nc_start[0] = t_offset_RHU + t;
        status_nc = nc_get_vara_int(ncID_RHU, varID_RHU, nc_start, nc_count, data_RHU);
        handle_error(status_nc, GP.FP_RHU);

        nc_start[0] = t_offset_WIN + t;
        status_nc = nc_get_vara_int(ncID_WIN, varID_WIN, nc_start, nc_count, data_WIN);
        handle_error(status_nc, GP.FP_WIN);

        nc_start[0] = t_offset_TEM_AVG + t;
        status_nc = nc_get_vara_int(ncID_TEM_AVG, varID_TEM_AVG, nc_start, nc_count, data_TEM_AVG);
        handle_error(status_nc, GP.FP_TEM_AVG);

        nc_start[0] = t_offset_TEM_MAX + t;
        status_nc = nc_get_vara_int(ncID_TEM_MAX, varID_TEM_MAX, nc_start, nc_count, data_TEM_MAX);
        handle_error(status_nc, GP.FP_TEM_MAX);

        nc_start[0] = t_offset_TEM_MIN + t;
        status_nc = nc_get_vara_int(ncID_TEM_MIN, varID_TEM_MIN, nc_start, nc_count, data_TEM_MIN);
        handle_error(status_nc, GP.FP_TEM_MIN);

        for (size_t i = 0; i < GEO_header.nrows; i++)
        {
            for (size_t j = 0; j < GEO_header.ncols; j++)
            {
                index_geo = i * GEO_header.ncols + j;
                if (*(data_SOILTYPE + index_geo) != GEO_header.NODATA_value)
                {
                    /********************** indexing **************************/
                    index_run = t * cell_counts_total + index_geo;
                    // printf("t: %d\n", t);
                    // printf("index_run: %d\n", index_run);
                    /************** weather forcing for cell ******************/
                    cell_PRE = *(data_PRE + index_geo) * scale_PRE / 1000; // [m]
                    cell_PRS = *(data_PRS + index_geo) * scale_PRS;
                    cell_SSD = *(data_SSD + index_geo) * scale_SSD;
                    cell_RHU = *(data_RHU + index_geo) * scale_RHU;
                    cell_WIN = *(data_WIN + index_geo) * scale_WIN;
                    cell_TEM_AVG = *(data_TEM_AVG + index_geo) * scale_TEM_AVG;
                    cell_TEM_MAX = *(data_TEM_MAX + index_geo) * scale_TEM_MAX;
                    cell_TEM_MIN = *(data_TEM_MIN + index_geo) * scale_TEM_MIN;
                    // printf(
                    //     "\n%8s%8s%8s%8s%8s%8s%8s%8s\n",
                    //     "PRE", "TEM_AVG", "TEM_MAX", "TEM_MIN", "WIN", "SSD", "RHU", "PRS");
                    // printf("%8.2f%8.2f%8.2f%8.2f%8.1f%8.0f%8.1f%8.1f\n",
                    //        cell_PRE * 1000, cell_TEM_AVG, cell_TEM_MAX, cell_TEM_MIN, cell_WIN, cell_SSD, cell_RHU, cell_PRS);
                    /**************** parameter preparation *****************/
                    cell_lat = *(data_lat + i);
                    cell_VEG_class = *(data_VEGTYPE + index_geo);
                    cell_SOIL_ID = *(data_SOILTYPE + index_geo);
                    cell_veg.CAN_FRAC = *(data_VEGFRAC + index_geo) / 100;
                    Lookup_VegLib_CELL(veglib, cell_VEG_class, &cell_veg);
                    Lookup_VegLib_CELL_MON(veglib, cell_VEG_class, month, &cell_veg);
                    Lookup_Soil_CELL(cell_SOIL_ID, &cell_soil, soillib, soilID);
                    // printf("VEG_CLASS: %d\n", cell_VEG_class);
                    // printf("cell_veg.Understory: %d\n", cell_veg.Understory);
                    // printf("cell_veg.d_u:%f\n", cell_veg.d_u);
                    // printf("cell_veg.CAN_FRAC: %f\n", cell_veg.CAN_FRAC);
                    
                    /******************* evapotranspiration *******************/
                    Soil_Fe = Soil_Desorption(
                        (data_SOIL + index_geo)->SM_Upper,
                        cell_soil.Topsoil->SatHydrauCond,
                        (1.0 / cell_soil.Topsoil->PoreSizeDisP),
                        (cell_soil.Topsoil->Porosity / 100.0),
                        cell_soil.Topsoil->Bubbling,
                        GP.STEP_TIME); // unit: m
                    // printf("Soil_Fe\n");
                    ET_CELL(
                        year, month, day, cell_lat,
                        cell_PRE, cell_TEM_AVG, cell_TEM_MIN, cell_TEM_MAX, cell_RHU, cell_PRS, cell_WIN,
                        ws_obs_z, cell_SSD,
                        &((data_RADIA + index_geo)->Rs),
                        &((data_RADIA + index_geo)->L_sky),
                        &((data_RADIA + index_geo)->Rno),
                        &((data_RADIA + index_geo)->Rno_short),
                        &((data_RADIA + index_geo)->Rnu),
                        &((data_RADIA + index_geo)->Rnu_short),
                        &((data_RADIA + index_geo)->Rns),
                        cell_veg.CAN_FRAC,
                        cell_veg.Albedo_o, cell_veg.Albedo_u, ALBEDO_SOIL,
                        cell_veg.LAI_o, cell_veg.LAI_u,
                        cell_veg.Rpc, cell_veg.rs_min_o, RS_MAX,
                        cell_veg.Rpc, cell_veg.rs_min_o, RS_MAX,

                        cell_veg.CAN_RZ, cell_veg.CAN_H,
                        cell_veg.d_o, cell_veg.z0_o,
                        cell_veg.d_u, cell_veg.z0_u,
                        (data_SOIL + index_geo)->SM_Upper,
                        cell_soil.Topsoil->WiltingPoint / 100,
                        cell_soil.Topsoil->FieldCapacity / 100,
                        Soil_Fe,
                        &((data_ET + index_geo)->Prec_throughfall),
                        &((data_ET + index_geo)->Prec_net),
                        &((data_ET + index_geo)->Ep),
                        &((data_ET + index_geo)->EI_o),
                        &((data_ET + index_geo)->ET_o),
                        &((data_ET + index_geo)->EI_u),
                        &((data_ET + index_geo)->ET_u),
                        &((data_ET + index_geo)->ET_s),
                        &((data_ET + index_geo)->Interception_o),
                        &((data_ET + index_geo)->Interception_u),
                        cell_veg.Understory,
                        GP.STEP_TIME);
                    // printf("\n test ET i = %d, j = %d\n", i, j);
                    // exit(-9);
                    /**************** unsaturated soil zone water movement *****************/
                    UnsaturatedWaterMove(
                        (data_ET + index_geo)->Prec_net / GP.STEP_TIME,
                        (data_ET + index_geo)->ET_o,
                        (data_ET + index_geo)->ET_u,
                        (data_ET + index_geo)->ET_s,
                        &((data_SOIL + index_geo)->SM_Upper),
                        &((data_SOIL + index_geo)->SM_Lower),
                        &((data_SOIL + index_geo)->SW_Infiltration),
                        &((data_SOIL + index_geo)->SW_Percolation_Upper),
                        &((data_SOIL + index_geo)->SW_Percolation_Lower),
                        (data_SOIL + index_geo)->SW_rise_lower,
                        (data_SOIL + index_geo)->SW_rise_upper,
                        &((data_SOIL + index_geo)->SW_SR_Infil),
                        &((data_SOIL + index_geo)->SW_SR_Satur),
                        Soil_d1,
                        Soil_d2,
                        &cell_soil,
                        GP.STEP_TIME);
                    // printf("UnsaturatedWaterMove\n");
                    /************************* save variables *************************/
                    // mandatory
                    *(out_SW_Run_Infil + index_run) = (int)((data_SOIL + index_geo)->SW_SR_Infil * 10000); // 0.1 mm
                    *(out_SW_Run_Satur + index_run) = (int)((data_SOIL + index_geo)->SW_SR_Satur * 10000);
                    // optional
                    if (outnl.Rs == 1)
                    {
                        *(out_Rs + index_geo) = (int)((data_RADIA + index_geo)->Rs * 10);
                    }
                    if (outnl.L_sky == 1)
                    {
                        *(out_L_sky + index_geo) = (int)((data_RADIA + index_geo)->L_sky * 10);
                    }
                    if (outnl.Rno == 1)
                    {
                        *(out_Rno + index_geo) = (int)((data_RADIA + index_geo)->Rno * 10);
                    }
                    if (outnl.Rnu == 1)
                    {
                        *(out_Rnu + index_geo) = (int)((data_RADIA + index_geo)->Rnu * 10);
                    }
                    if (outnl.Ep == 1)
                    {
                        *(out_Ep + index_geo) = (int)((data_ET + index_geo)->Ep * GP.STEP_TIME * 10000);
                    }
                    if (outnl.EI_o == 1)
                    {
                        *(out_EI_o + index_geo) = (int)((data_ET + index_geo)->EI_o * 10000);
                    }
                    if (outnl.EI_u == 1)
                    {
                        *(out_EI_u + index_geo) = (int)((data_ET + index_geo)->EI_u * 10000);
                    }
                    if (outnl.ET_o == 1)
                    {
                        *(out_ET_o + index_geo) = (int)((data_ET + index_geo)->ET_o * 10000);
                    }
                    if (outnl.ET_u == 1)
                    {
                        *(out_ET_u + index_geo) = (int)((data_ET + index_geo)->ET_u * 10000);
                    }
                    if (outnl.ET_s == 1)
                    {
                        *(out_ET_s + index_geo) = (int)((data_ET + index_geo)->ET_s * 10000);
                    }
                    if (outnl.Prec_net == 1)
                    {
                        *(out_Prec_net + index_geo) = (int)((data_ET + index_geo)->Prec_net * 10000);
                    }
                    if (outnl.SM_Upper == 1)
                    {
                        *(out_SM_Upper + index_geo) = (int)((data_SOIL + index_geo)->SM_Upper * 100);
                    }
                    if (outnl.SM_Lower == 1)
                    {
                        *(out_SM_Lower + index_geo) = (int)((data_SOIL + index_geo)->SM_Lower * 100);
                    }
                    if (outnl.SW_Infiltration == 1)
                    {
                        *(out_SW_Infiltration + index_geo) = (int)((data_SOIL + index_geo)->SW_Infiltration * 10000);
                    }
                    if (outnl.SW_Percolation_Lower == 1)
                    {
                        *(out_SW_Percolation_Lower + index_geo) = (int)((data_SOIL + index_geo)->SW_Percolation_Lower * 10000);
                    }
                    if (outnl.SW_Percolation_Upper == 1)
                    {
                        *(out_SW_Percolation_Upper + index_geo) = (int)((data_SOIL + index_geo)->SW_Percolation_Upper * 10000);
                    }
                }
            }
        }
        /**************** water movement in saturated soil zone *****************/

        Soil_Satu_Move(
            data_DEM,
            data_STR,
            data_SOILTYPE,
            &data_STREAM,
            &data_SOIL,
            soillib,
            soilID,
            Soil_Thickness,
            Soil_d1,
            Soil_d2,
            stream_depth,
            stream_width,
            GEO_header.NODATA_value,
            GEO_header.ncols,
            GEO_header.nrows,
            (double) cellsize_m,
            GP.STEP_TIME);
        
        int tog;
        tog = outnl.SW_SUB_Qin + outnl.SW_SUB_Qout + outnl.SW_SUB_z + outnl.SW_SUB_rise_lower + outnl.SW_SUB_rise_upper + outnl.SW_SUB_rf; 
        if (tog > 0)
        {
            for (size_t i = 0; i < GEO_header.nrows; i++)
            {
                for (size_t j = 0; j < GEO_header.ncols; j++)
                {
                    index_geo = i * GEO_header.ncols + j;
                    if (*(data_DEM + index_geo) != GEO_header.NODATA_value)
                    {
                        if (outnl.SW_SUB_Qin == 1)
                        {
                            *(out_SW_SUB_Qin + index_geo) = (int)((data_SOIL + index_geo)->Qin / cellarea_m * GP.STEP_TIME * 10000);
                        }
                        if (outnl.SW_SUB_Qout == 1)
                        {
                            *(out_SW_SUB_Qout + index_geo) = (int)((data_SOIL + index_geo)->Qout / cellarea_m * GP.STEP_TIME * 10000);
                        }
                        if (outnl.SW_SUB_z == 1)
                        {
                            *(out_SW_SUB_z + index_geo) = (int)((data_SOIL + index_geo)->z * 100);
                        }
                        if (outnl.SW_SUB_rise_lower == 1)
                        {
                            *(out_SW_SUB_rise_lower + index_geo) = (int)((data_SOIL + index_geo)->SW_rise_lower * 10000);
                        }
                        if (outnl.SW_SUB_rise_upper == 1)
                        {
                            *(out_SW_SUB_rise_upper + index_geo) = (int)((data_SOIL + index_geo)->SW_rise_upper * 10000);
                        }
                        if (outnl.SW_SUB_rf == 1)
                        {
                            *(out_SW_SUB_rf + index_geo) = (int)((data_SOIL + index_geo)->SW_rf * 10000);
                        }
                    }
                }
            }
        }
        
        /********************* river channel flow routing ****************/
        Channel_Network_Routing(
            &data_STREAM,
            data_STR,
            GEO_header.ncols,
            GEO_header.nrows,
            GP.STEP_TIME);
        if (outnl.SW_SUB_Qc + outnl.Q_Channel > 0)
        {
            for (size_t i = 0; i < GEO_header.nrows; i++)
            {
                for (size_t j = 0; j < GEO_header.ncols; j++)
                {
                    index_geo = i * GEO_header.ncols + j;
                    if (*(data_STR + index_geo) == 1)
                    {
                        if (outnl.SW_SUB_Qc == 1)
                        {
                            *(out_SW_SUB_Qc + index_geo) = (int)((data_STREAM + index_geo)->Qc / cellarea_m * GP.STEP_TIME * 10000);
                        }
                        if (outnl.Q_Channel == 1)
                        {
                            *(out_Q_Channel + index_geo) = (int)((data_STREAM + index_geo)->Qout / 3600 * 1000);
                        }
                    }
                }
            }
        }
        
        for (size_t s = 0; s < outlet_count; s++)
        {
            *(Qout_Sub + s * time_steps_run + t) = (data_STREAM + outlet_index_row[s] * GEO_header.ncols + outlet_index_col[s])->Qout;
        }
        /********************* write state variable to .nc ***************/
        Write_Outnamelist(
            t,
            outnl,
            outnl_ncid,
            GEO_header,
            &out_Rs,
            &out_L_sky,
            &out_Rno,
            &out_Rnu,
            &out_Ep,
            &out_EI_o,
            &out_EI_u,
            &out_ET_o,
            &out_ET_u,
            &out_ET_s,
            &out_Interception_o,
            &out_Interception_u,
            &out_Prec_net,
            &out_SM_Upper,
            &out_SM_Lower,
            &out_SW_Percolation_Upper,
            &out_SW_Percolation_Lower,
            &out_SW_Infiltration,
            &out_SW_SUB_Qin,
            &out_SW_SUB_Qout,
            &out_SW_SUB_z,
            &out_SW_SUB_rise_upper,
            &out_SW_SUB_rise_lower,
            &out_SW_SUB_rf,
            &out_SW_SUB_Qc,
            &out_Q_Channel);
        /********************* next iteration ****************/
        t += 1;
        run_time += 3600 * GP.STEP_TIME;
    }
    OUTVAR_nc_close(outnl, outnl_ncid);
    /***************************************************************************************************
     *                               export the variables: runoff generation
     ****************************************************************************************************/
    Write2NC_Outnamelist(outnl, time_steps_run, &out_SW_Run_Infil, &out_SW_Run_Satur, GP);
    printf(" Done!\n");
    exit(-9);
    /************************ surface runoff routing **********************/
    // UH method for multiple outlets
    time(&tm); printf("--------- %s xHM overland runoff routing with UH method: ", DateString(&tm));
    int index_UH_gap;
    index_UH_gap = 0;
    for (size_t s = 0; s < outlet_count; s++)
    {
        UH_Routing(
            out_SW_Run_Infil, 
            data_UH + index_UH_gap,
            Qout_SF_Infil + time_steps_run * s,
            UH_steps[s],
            GEO_header.ncols,
            GEO_header.nrows,
            time_steps_run,
            cellsize_m,
            GEO_header.NODATA_value,
            GP.STEP_TIME);
        UH_Routing(
            out_SW_Run_Satur, 
            data_UH + index_UH_gap,
            Qout_SF_Satur + time_steps_run * s,
            UH_steps[s],
            GEO_header.ncols,
            GEO_header.nrows,
            time_steps_run,
            cellsize_m,
            GEO_header.NODATA_value,
            GP.STEP_TIME);
        index_UH_gap += UH_steps[s] * cell_counts_total;
    }
    printf("Done!\n");
    /******************** total discharge at outlets ************************/
    Route_Outlet(
        Qout_SF_Infil,
        Qout_SF_Satur,
        Qout_Sub,
        Qout_outlet,
        outlet_count,
        time_steps_run);
    time(&tm); printf("--------- %s xHM write the output variables to NetCDF files: ", DateString(&tm));
    Write_Qout(
        outnl,
        GP.PATH_OUT,
        Qout_SF_Infil,
        Qout_SF_Satur,
        Qout_Sub,
        Qout_outlet,
        outlet_index_row,
        outlet_index_col,
        outlet_count,
        time_steps_run);
    printf("Done! \n");
    
    /***************************************************************************************************
     *                               finalize the program
     ****************************************************************************************************/
    free(data_lon);free(data_lat);
    free(data_DEM);free(data_FDR);free(data_SOILTYPE);free(data_STR);free(data_VEGFRAC);free(data_VEGTYPE);
    free(data_PRE);free(data_PRS);free(data_RHU);free(data_SSD);free(data_WIN);free(data_TEM_AVG);free(data_TEM_MAX);free(data_TEM_MIN);
    free(data_UH);
    free(data_RADIA);free(data_SOIL);free(data_STREAM);free(data_ET);

    nc_close(ncID_PRE);
    nc_close(ncID_PRS);
    nc_close(ncID_SSD);
    nc_close(ncID_RHU);
    nc_close(ncID_WIN);
    nc_close(ncID_TEM_AVG);
    nc_close(ncID_TEM_MAX);
    nc_close(ncID_TEM_MIN);
    nc_close(ncID_GEO);
    nc_close(ncID_UH);
    time(&tm); printf("--------- %s xHM modelling: Done!\n", DateString(&tm));
    return 1;
}

void malloc_error(
    int *data)
{
    if (data == NULL)
    {
        printf("memory allocation failed!\n");
        exit(-3);
    }
}

char *DateString(
    time_t *tm
)
{
    struct tm *ptm;
    static char buf[30] = {0};
    ptm = localtime(tm);
    ptm->tm_isdst = 0; 
    strftime(buf, 30, "%Y-%m-%d %H:%M:%S", ptm);
    return buf;
}

