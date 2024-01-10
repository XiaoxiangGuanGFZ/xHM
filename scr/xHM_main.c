/*
 * SUMMARY:      xHM_main.c
 * USAGE:        hydrological modeling module
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Jan-2024
 * DESCRIPTION:  simulate the multiple hydrological processes:
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
#include "Soil_UnsaturatedMove.h"

void malloc_error(
    int *data
);

int main(int argc, char *argv[])
{
    /******************************************************************************
     *                        read global parameter file
     ******************************************************************************/
    GLOBAL_PARA GP;
    Initialize_GlobalPara(&GP);
    Import_GlobalPara(*(++argv), &GP);
    double ws_obs_z = 10.0;/* the height of wind measurement */
    char WS_OUT[MAXCHAR];
    strcpy(WS_OUT, GP.PATH_OUT);
    OUT_NAME_LIST outnl;
    Initialize_Outnamelist(&outnl);
    Import_Outnamelist(GP.FP_OUTNAMELIST, &outnl);
    /*****************************************************************************
     *                          model simulation period
     ******************************************************************************/
    int time_steps_run;  // number of simulation steps
    time_t start_time, end_time;
    struct tm tm_start, tm_end;
    tm_start.tm_hour = GP.START_HOUR + 1; tm_end.tm_hour = GP.END_HOUR + 1;
    tm_start.tm_min = 0; tm_end.tm_min = 0;
    tm_start.tm_sec = 0; tm_end.tm_sec = 0;
    tm_start.tm_mday = GP.START_DAY; tm_end.tm_mday = GP.END_DAY; 
    tm_start.tm_mon = GP.START_MONTH - 1; tm_end.tm_mon = GP.END_MONTH - 1;
    tm_start.tm_year = GP.START_YEAR - 1900; tm_end.tm_year = GP.END_YEAR - 1900;
    // Daylight Saving Time not in effect;Specify UTC time zone
    tm_start.tm_isdst = 0; tm_end.tm_isdst = 0; 
    start_time = mktime(&tm_start); end_time = mktime(&tm_end);
    if (start_time > end_time)
    {
        printf("Error: invalid start time or end time for the model running. Please check the global parameter file!\n");
        exit(-2);
    }
    
    time_steps_run = (end_time - start_time) / (3600 * GP.STEP_TIME) + 1;
    printf("time_steps_run: %d\n", time_steps_run);
    /******************************************************************************
     *                              read GEO info
     ******************************************************************************/
    int status_nc;
    int ncID_GEO;
    ST_Header GEO_header;
    nc_open(GP.FP_GEO, NC_NOWRITE, &ncID_GEO);
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "ncols", &GEO_header.ncols);
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "nrows", &GEO_header.nrows);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "xllcorner", &GEO_header.xllcorner);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "yllcorner", &GEO_header.yllcorner);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "cellsize", &GEO_header.cellsize);
    int cell_counts_total;
    cell_counts_total = GEO_header.ncols * GEO_header.nrows;
    printf("cell_counts_total: %d\n", cell_counts_total);
    printf("ncols: %d\nnrows: %d\nxllcorner: %.12f\nyllcorner: %.12f\ncellsize: %.12f\n",
           GEO_header.ncols, GEO_header.nrows, GEO_header.xllcorner, GEO_header.yllcorner, GEO_header.cellsize);
    
    int varID_VEGTYPE, varID_VEGFRAC, varID_lon, varID_lat, varID_SOILTYPE;
    int *data_VEGTYPE;
    int *data_VEGFRAC;
    int *data_SOILTYPE;
    double *data_lon;
    double *data_lat;

    data_VEGTYPE = (int *)malloc(sizeof(int) * cell_counts_total);
    data_VEGFRAC = (int *)malloc(sizeof(int) * cell_counts_total);
    data_SOILTYPE = (int *)malloc(sizeof(int) * cell_counts_total);
    data_lat = (double *)malloc(sizeof(double) * GEO_header.nrows);
    data_lon = (double *)malloc(sizeof(double) * GEO_header.ncols);
    
    nc_inq_varid(ncID_GEO, "VEGTYPE", &varID_VEGTYPE);
    nc_inq_varid(ncID_GEO, "VEGFRAC", &varID_VEGFRAC);
    nc_inq_varid(ncID_GEO, "SOILTYPE", &varID_SOILTYPE);
    nc_inq_varid(ncID_GEO, "lon", &varID_lon);
    nc_inq_varid(ncID_GEO, "lat", &varID_lat);

    nc_get_var_int(ncID_GEO, varID_VEGTYPE, data_VEGTYPE);
    nc_get_var_int(ncID_GEO, varID_VEGFRAC, data_VEGFRAC);
    nc_get_var_int(ncID_GEO, varID_SOILTYPE, data_SOILTYPE);
    nc_get_var_double(ncID_GEO, varID_lon, data_lon);
    nc_get_var_double(ncID_GEO, varID_lat, data_lat);
    printf("lat: %.2f\n", *(data_lat + 25));
    printf("SOIL: %d\n", *(data_SOILTYPE + 25*GEO_header.ncols + 25));
    printf("VEG: %d\n", *(data_VEGTYPE + 25*GEO_header.ncols + 25));
    printf("VEGFRAC: %d\n", *(data_VEGFRAC + 25*GEO_header.ncols + 25));
    
    // check the GEO data
    Check_GEO(ncID_GEO);
    // import the vegetation library
    ST_VegLib veglib[11];
    Import_veglib(GP.FP_VEGLIB, veglib);

    // import the soil property library abd HWSD IDs
    ST_SoilLib soillib[13];
    Import_soillib(GP.FP_SOILLIB, soillib);
    ST_SoilID soilID[1000];
    Import_soil_HWSD_ID(GP.FP_SOIL_HWSD_ID, soilID);
    
    /******************************************************************************
     *                      read gridded weather data
     ******************************************************************************/
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
    
    status_nc = nc_open(GP.FP_PRE, NC_NOWRITE, &ncID_PRE); handle_error(status_nc, GP.FP_PRE);
    status_nc = nc_open(GP.FP_PRS, NC_NOWRITE, &ncID_PRS); handle_error(status_nc, GP.FP_PRS);
    status_nc = nc_open(GP.FP_RHU, NC_NOWRITE, &ncID_RHU); handle_error(status_nc, GP.FP_RHU);
    status_nc = nc_open(GP.FP_SSD, NC_NOWRITE, &ncID_SSD); handle_error(status_nc, GP.FP_SSD);
    status_nc = nc_open(GP.FP_WIN, NC_NOWRITE, &ncID_WIN); handle_error(status_nc, GP.FP_WIN);
    status_nc = nc_open(GP.FP_TEM_AVG, NC_NOWRITE, &ncID_TEM_AVG); handle_error(status_nc, GP.FP_TEM_AVG);
    status_nc = nc_open(GP.FP_TEM_MAX, NC_NOWRITE, &ncID_TEM_MAX); handle_error(status_nc, GP.FP_TEM_MAX);
    status_nc = nc_open(GP.FP_TEM_MIN, NC_NOWRITE, &ncID_TEM_MIN); handle_error(status_nc, GP.FP_TEM_MIN);
    /* check the weather datasets: compatibility, consistency */
    Check_weather(ncID_PRE, ncID_PRS, ncID_RHU, ncID_SSD, ncID_WIN, ncID_TEM_AVG, ncID_TEM_MAX, ncID_TEM_MIN, start_time, end_time, GP.STEP_TIME);

    int dimID_time;
    int varID_time;
    // the starting time of the variable series
    long t_PRE, t_PRS, t_SSD, t_RHU, t_WIN, t_TEM_AVG, t_TEM_MAX, t_TEM_MIN;
    // the length (steps) of the variable series
    int time_steps_PRE, time_steps_PRS, time_steps_RHU, time_steps_SSD, time_steps_WIN, time_steps_TEM_AVG, time_steps_TEM_MAX, time_steps_TEM_MIN;
    
    size_t index = 0;

    nc_inq_varid(ncID_PRE, "time", &varID_time); nc_get_var1_long(ncID_PRE, varID_time, &index, &t_PRE); 
    nc_inq_dimid(ncID_PRE, "time", &dimID_time); nc_inq_dimlen(ncID_PRE, dimID_time, &time_steps_PRE);

    nc_inq_varid(ncID_PRS, "time", &varID_time); nc_get_var1_long(ncID_PRS, varID_time, &index, &t_PRS);
    nc_inq_dimid(ncID_PRS, "time", &dimID_time); nc_inq_dimlen(ncID_PRS, dimID_time, &time_steps_PRS);

    nc_inq_varid(ncID_SSD, "time", &varID_time); nc_get_var1_long(ncID_SSD, varID_time, &index, &t_SSD);
    nc_inq_dimid(ncID_SSD, "time", &dimID_time); nc_inq_dimlen(ncID_SSD, dimID_time, &time_steps_SSD);

    nc_inq_varid(ncID_RHU, "time", &varID_time); nc_get_var1_long(ncID_RHU, varID_time, &index, &t_RHU);
    nc_inq_dimid(ncID_RHU, "time", &dimID_time); nc_inq_dimlen(ncID_RHU, dimID_time, &time_steps_RHU);

    nc_inq_varid(ncID_WIN, "time", &varID_time); nc_get_var1_long(ncID_WIN, varID_time, &index, &t_WIN);
    nc_inq_dimid(ncID_WIN, "time", &dimID_time); nc_inq_dimlen(ncID_WIN, dimID_time, &time_steps_WIN);

    nc_inq_varid(ncID_TEM_AVG, "time", &varID_time); nc_get_var1_long(ncID_TEM_AVG, varID_time, &index, &t_TEM_AVG);
    nc_inq_dimid(ncID_TEM_AVG, "time", &dimID_time); nc_inq_dimlen(ncID_TEM_AVG, dimID_time, &time_steps_TEM_AVG);

    nc_inq_varid(ncID_TEM_MAX, "time", &varID_time); nc_get_var1_long(ncID_TEM_MAX, varID_time, &index, &t_TEM_MAX);
    nc_inq_dimid(ncID_TEM_MAX, "time", &dimID_time); nc_inq_dimlen(ncID_TEM_MAX, dimID_time, &time_steps_TEM_MAX);

    nc_inq_varid(ncID_TEM_MIN, "time", &varID_time); nc_get_var1_long(ncID_TEM_MIN, varID_time, &index, &t_TEM_MIN); 
    nc_inq_dimid(ncID_TEM_MIN, "time", &dimID_time); nc_inq_dimlen(ncID_TEM_MIN, dimID_time, &time_steps_TEM_MIN);

    // printf("length of PRE observation: %d\n", time_steps_PRE);
    printf("start time of TEM_AVG: %ld\n", t_TEM_AVG);
    printf("length of TEM_AVG observation: %d\n", time_steps_TEM_AVG);
    printf("length of TEM_PRE observation: %d\n", time_steps_PRE);

    status_nc = nc_inq_varid(ncID_PRE, "PRE", &varID_PRE); handle_error(status_nc, GP.FP_PRE);
    status_nc = nc_inq_varid(ncID_PRS, "PRS", &varID_PRS); handle_error(status_nc, GP.FP_PRS);
    status_nc = nc_inq_varid(ncID_SSD, "SSD", &varID_SSD); handle_error(status_nc, GP.FP_SSD);
    status_nc = nc_inq_varid(ncID_RHU, "RHU", &varID_RHU); handle_error(status_nc, GP.FP_RHU);
    status_nc = nc_inq_varid(ncID_WIN, "WIN", &varID_WIN); handle_error(status_nc, GP.FP_WIN);
    status_nc = nc_inq_varid(ncID_TEM_AVG, "TEM_AVG", &varID_TEM_AVG); handle_error(status_nc, GP.FP_TEM_AVG);
    status_nc = nc_inq_varid(ncID_TEM_MAX, "TEM_MAX", &varID_TEM_MAX); handle_error(status_nc, GP.FP_TEM_MAX);
    status_nc = nc_inq_varid(ncID_TEM_MIN, "TEM_MIN", &varID_TEM_MIN); handle_error(status_nc, GP.FP_TEM_MIN);
    printf("malloc for weather data: ");
    printf("memory size required in total: %.2f GB\n",
           (float)sizeof(int) * time_steps_PRE * cell_counts_total * 8 / 1024 / 1024 / 1024);
    data_PRE = (int *)malloc(sizeof(int) * time_steps_PRE * cell_counts_total); malloc_error(data_PRE);
    data_PRS = (int *)malloc(sizeof(int) * time_steps_PRS * cell_counts_total); malloc_error(data_PRS);
    data_SSD = (int *)malloc(sizeof(int) * time_steps_SSD * cell_counts_total); malloc_error(data_SSD);
    data_RHU = (int *)malloc(sizeof(int) * time_steps_RHU * cell_counts_total); malloc_error(data_RHU);
    data_WIN = (int *)malloc(sizeof(int) * time_steps_WIN * cell_counts_total); malloc_error(data_WIN);
    data_TEM_AVG = (int *)malloc(sizeof(int) * time_steps_TEM_AVG * cell_counts_total); malloc_error(data_TEM_AVG);
    data_TEM_MAX = (int *)malloc(sizeof(int) * time_steps_TEM_MAX * cell_counts_total); malloc_error(data_TEM_MAX);
    data_TEM_MIN = (int *)malloc(sizeof(int) * time_steps_TEM_MIN * cell_counts_total); malloc_error(data_TEM_MIN);
    
    status_nc = nc_get_var_int(ncID_PRE, varID_PRE, data_PRE); handle_error(status_nc, GP.FP_PRE);
    status_nc = nc_get_var_int(ncID_PRS, varID_PRS, data_PRS); handle_error(status_nc, GP.FP_PRS);
    status_nc = nc_get_var_int(ncID_SSD, varID_SSD, data_SSD); handle_error(status_nc, GP.FP_SSD);
    status_nc = nc_get_var_int(ncID_RHU, varID_RHU, data_RHU); handle_error(status_nc, GP.FP_RHU);
    status_nc = nc_get_var_int(ncID_WIN, varID_WIN, data_WIN); handle_error(status_nc, GP.FP_WIN);
    status_nc = nc_get_var_int(ncID_TEM_AVG, varID_TEM_AVG, data_TEM_AVG); handle_error(status_nc, GP.FP_TEM_AVG);
    status_nc = nc_get_var_int(ncID_TEM_MAX, varID_TEM_MAX, data_TEM_MAX); handle_error(status_nc, GP.FP_TEM_MAX);
    status_nc = nc_get_var_int(ncID_TEM_MIN, varID_TEM_MIN, data_TEM_MIN); handle_error(status_nc, GP.FP_TEM_MIN);
    printf("check weather data at row15 col25:\n");
    printf("PRE: %d\n", *(data_PRE + 25*GEO_header.ncols + 25));
    printf("RHU: %d\n", *(data_RHU + 25*GEO_header.ncols + 25));
    printf("TEM_AVG: %d\n", *(data_TEM_AVG + 25*GEO_header.ncols + 25));
    nc_get_att_int(ncID_PRE, varID_PRE, "NODATA_value", &GEO_header.NODATA_value);

    double scale_PRE, scale_PRS, scale_SSD, scale_RHU, scale_WIN, scale_TEM_AVG, scale_TEM_MAX, scale_TEM_MIN;
    status_nc = nc_get_att_double(ncID_PRE, varID_PRE, "scale_factor", &scale_PRE); handle_error(status_nc, GP.FP_PRE);
    status_nc = nc_get_att_double(ncID_PRS, varID_PRS, "scale_factor", &scale_PRS); handle_error(status_nc, GP.FP_PRS);
    status_nc = nc_get_att_double(ncID_SSD, varID_SSD, "scale_factor", &scale_SSD); handle_error(status_nc, GP.FP_SSD);
    status_nc = nc_get_att_double(ncID_RHU, varID_RHU, "scale_factor", &scale_RHU); handle_error(status_nc, GP.FP_RHU);
    status_nc = nc_get_att_double(ncID_WIN, varID_WIN, "scale_factor", &scale_WIN); handle_error(status_nc, GP.FP_WIN);
    status_nc = nc_get_att_double(ncID_TEM_AVG, varID_TEM_AVG, "scale_factor", &scale_TEM_AVG); handle_error(status_nc, GP.FP_TEM_AVG);
    status_nc = nc_get_att_double(ncID_TEM_MAX, varID_TEM_MAX, "scale_factor", &scale_TEM_MAX); handle_error(status_nc, GP.FP_TEM_MAX);
    status_nc = nc_get_att_double(ncID_TEM_MIN, varID_TEM_MIN, "scale_factor", &scale_TEM_MIN); handle_error(status_nc, GP.FP_TEM_MIN);
    printf("check scale_factor:\n");
    printf("PRE: %.1f\n", scale_PRE);
    printf("RHU: %.1f\n", scale_RHU);
    printf("TEM_AVG: %.1f\n", scale_TEM_AVG);
    printf("import weather data from NetCDF files: DONE!\n");
    /***********************************************************************************
     *                          set model running period
     ************************************************************************************/
    int t = 0;
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
    printf("t_offset:\n");
    printf("PRE: %d\n", t_offset_PRE);
    printf("WIN: %d\n", t_offset_WIN);
    printf("TEM_AVG: %d\n", t_offset_TEM_AVG);
    time_t run_time;
    run_time = start_time;
    int index_PRE, index_PRS, index_SSD, index_RHU, index_WIN, index_TEM_AVG, index_TEM_MAX, index_TEM_MIN;
    int index_geo;
    int index_run;

    /***********************************************************************************
     *                      surface runoff routing - UH
     ************************************************************************************/
    UH_Generation(GP.FP_GEO, GP.FP_UH, GP.STEP_TIME,
                  GP.Velocity_avg, GP.Velocity_max, GP.Velocity_min, GP.b, GP.c);

    /***********************************************************************************
     *              define and initialize the intermediate variables
     ***********************************************************************************/
    CELL_VAR_RADIA *data_RADIA;
    data_RADIA = (CELL_VAR_RADIA *)malloc(sizeof(CELL_VAR_RADIA) * cell_counts_total);
    
    CELL_VAR_ET *data_ET;
    data_ET = (CELL_VAR_ET *)malloc(sizeof(CELL_VAR_ET) * cell_counts_total);

    CELL_VAR_SOIL *data_SOIL;
    data_SOIL = (CELL_VAR_SOIL *)malloc(sizeof(CELL_VAR_SOIL) * cell_counts_total);

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

    /***********************************************************************************
     *              define the ourput variables (results) from simulation
     ***********************************************************************************/
    char FP_OUT_VAR[MAXCHAR]="";
    int *out_Rs, *out_L_sky, *out_Rno, *out_Rnu;
    int *out_Ep, *out_EI_o, *out_EI_u, *out_ET_o, *out_ET_u, *out_ET_s;
    int *out_Interception_o, *out_Interception_u, *out_Prec_net;
    malloc_Outnamelist(
        outnl, cell_counts_total, time_steps_run,
        &out_Rs, &out_L_sky, &out_Rno, &out_Rnu,
        &out_Ep, &out_EI_o, &out_EI_u, &out_ET_o, &out_ET_u, &out_ET_s,
        &out_Interception_o, &out_Interception_u, &out_Prec_net);

    /***********************************************************************************
     *                       define the iteration variables
     ***********************************************************************************/

    double cell_PRE;
    double cell_WIN;
    double cell_SSD;
    double cell_RHU;
    double cell_PRS;
    double cell_TEM_AVG;
    double cell_TEM_MAX;
    double cell_TEM_MIN;
    
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

    double Soil_thickness_upper = 0.2;
    double Soil_thickness_lower = 0.5;
    /***********************************************************************************
     *                       xHM model iteration
     ***********************************************************************************/
    while (run_time <= end_time)
    {
        tm_run = gmtime(&run_time);
        year = tm_run->tm_year + 1900;
        month = tm_run->tm_mon + 1;
        day = tm_run->tm_mday;
        printf("%d-%02d-%02d\n",year,month,day);
        for (size_t i = 0; i < GEO_header.nrows; i++)
        {
            for (size_t j = 0; j < GEO_header.ncols; j++)
            {
                index_geo = i * GEO_header.ncols + j;
                if (*(data_SOILTYPE + index_geo) != GEO_header.NODATA_value)
                {
                    /********************** indexing **************************/
                    index_run = t * cell_counts_total + i * GEO_header.ncols + j;
                    index_PRE = (t + t_offset_PRE) * cell_counts_total + index_geo;
                    index_PRS = (t + t_offset_PRS) * cell_counts_total + index_geo;
                    index_SSD = (t + t_offset_SSD) * cell_counts_total + index_geo;
                    index_RHU = (t + t_offset_RHU) * cell_counts_total + index_geo;
                    index_WIN = (t + t_offset_WIN) * cell_counts_total + index_geo;
                    index_TEM_AVG = (t + t_offset_TEM_AVG) * cell_counts_total + index_geo;
                    index_TEM_MAX = (t + t_offset_TEM_MAX) * cell_counts_total + index_geo;
                    index_TEM_MIN = (t + t_offset_TEM_MIN) * cell_counts_total + index_geo;
                    
                    /************** weather forcing for cell ******************/
                    cell_PRE = *(data_PRE + index_PRE) * scale_PRE / 1000;  // [m]
                    cell_PRS = *(data_PRS + index_PRS) * scale_PRS;
                    cell_SSD = *(data_SSD + index_SSD) * scale_SSD;
                    cell_RHU = *(data_RHU + index_RHU) * scale_RHU;
                    cell_WIN = *(data_WIN + index_WIN) * scale_WIN;
                    cell_TEM_AVG = *(data_TEM_AVG + index_TEM_AVG) * scale_TEM_AVG;
                    cell_TEM_MAX = *(data_TEM_MAX + index_TEM_MAX) * scale_TEM_MAX;
                    cell_TEM_MIN = *(data_TEM_MIN + index_TEM_MIN) * scale_TEM_MIN;
                    // printf(
                    //     "%8s%8s%8s%8s%8s%8s%8s%8s\n",
                    //     "PRE", "TEM_AVG", "TEM_MAX", "TEM_MIN", "WIN", "SSD", "RHU", "PRS");
                    // printf("%8.2f%8.2f%8.2f%8.2f%8.1f%8.0f%8.1f%8.1f\n",
                    //        cell_PRE * 1000, cell_TEM_AVG, cell_TEM_MAX, cell_TEM_MIN, cell_WIN, cell_SSD, cell_RHU, cell_PRS);
                    /**************** parameter preparation *****************/
                    cell_lat = *(data_lat + i);
                    cell_VEG_class = *(data_VEGTYPE + index_geo);
                    printf("VEG_CLASS: %d\n", cell_VEG_class);
                    cell_SOIL_ID = *(data_SOILTYPE + index_geo);
                    cell_veg.CAN_FRAC = *(data_VEGFRAC + index_geo) / 100;
                    Lookup_VegLib_CELL(veglib, cell_VEG_class, &cell_veg);
                    Lookup_VegLib_CELL_MON(veglib, cell_VEG_class, month, &cell_veg);
                    Lookup_Soil_CELL(cell_SOIL_ID, &cell_soil, soillib, soilID);
                    /******************* evapotranspiration *******************/
                    Soil_Fe = Soil_Desorption(
                        (data_SOIL + index_geo)->SM_Upper,
                        cell_soil.Topsoil->SatHydrauCond,
                        (1.0 / cell_soil.Topsoil->PoreSizeDisP),
                        (cell_soil.Topsoil->Porosity / 100.0),
                        cell_soil.Topsoil->Bubbling,
                        GP.STEP_TIME); // unit: m
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

                    /**************** unsaturated soil zone water movement *****************/
                    UnsaturatedWaterMove(
                        (data_ET + index_geo)->Prec_net,
                        (data_ET + index_geo)->ET_o,
                        (data_ET + index_geo)->ET_u,
                        (data_ET + index_geo)->ET_s,
                        &((data_SOIL + index_geo)->SM_Upper),
                        &((data_SOIL + index_geo)->SM_Lower),
                        &((data_SOIL + index_geo)->SW_Infiltration),
                        &((data_SOIL + index_geo)->SW_Percolation_Upper),
                        &((data_SOIL + index_geo)->SW_Percolation_Lower),
                        (data_SOIL + index_geo)->SWV_gw,
                        (data_SOIL + index_geo)->SWV_rf,
                        &((data_SOIL + index_geo)->SW_SR_Infil),
                        &((data_SOIL + index_geo)->SW_SR_Satur),
                        Soil_thickness_upper,
                        Soil_thickness_lower,
                        &cell_soil,
                        GP.STEP_TIME);

                    /************************* save variables *************************/
                    if (outnl.Rs == 1)
                    {
                        *(out_Rs + index_run) = (int)((data_RADIA + index_geo)->Rs * 10);
                    }
                    if (outnl.L_sky == 1)
                    {
                        *(out_L_sky + index_run) = (int)((data_RADIA + index_geo)->L_sky * 10);
                    }
                    if (outnl.Rno == 1)
                    {
                        *(out_Rno + index_run) = (int)((data_RADIA + index_geo)->Rno * 10);
                    }
                    if (outnl.Rnu == 1)
                    {
                        *(out_Rnu + index_run) = (int)((data_RADIA + index_geo)->Rnu * 10);
                    }
                    if (outnl.Ep == 1)
                    {
                        *(out_Ep + index_run) = (int)((data_ET + index_geo)->Ep * GP.STEP_TIME * 10000);
                    }
                    if (outnl.EI_o == 1)
                    {
                        *(out_EI_o + index_run) = (int)((data_ET + index_geo)->EI_o * 10000);
                    }
                    if (outnl.EI_u == 1)
                    {
                        *(out_EI_u + index_run) = (int)((data_ET + index_geo)->EI_u * 10000);
                    }
                    if (outnl.ET_o == 1)
                    {
                        *(out_ET_o + index_run) = (int)((data_ET + index_geo)->ET_o * 10000);
                    }
                    if (outnl.ET_u == 1)
                    {
                        *(out_ET_u + index_run) = (int)((data_ET + index_geo)->ET_u * 10000);
                    }
                    if (outnl.ET_s == 1)
                    {
                        *(out_ET_s + index_run) = (int)((data_ET + index_geo)->ET_s * 10000);
                    }
                    if (outnl.Prec_net == 1)
                    {
                        *(out_Prec_net + index_run) = (int)((data_ET + index_geo)->Prec_net * 10000);
                    }
                }
            }
        }
        /**************** water movement in saturated soil zone *****************/

        /********************* river channel flow routing ****************/
        
        t += 1;
        run_time += 3600 * GP.STEP_TIME;
    }
    /************************ surface runoff routing **********************/

    /***************************************************************************************************
     *                               export the variables
     ****************************************************************************************************/
    Write2NC_Outnamelist(
        outnl, time_steps_run,
        &out_Rs, &out_L_sky, &out_Rno, &out_Rnu,
        &out_Ep, &out_EI_o, &out_EI_u, &out_ET_o, &out_ET_u, &out_ET_s,
        &out_Interception_o, &out_Interception_u, &out_Prec_net,
        GP
    );

    /***************************************************************************************************
     *                               finalize the program
     ****************************************************************************************************/
    nc_close(ncID_PRE); nc_close(ncID_PRS); nc_close(ncID_SSD); nc_close(ncID_RHU); 
    nc_close(ncID_WIN); nc_close(ncID_TEM_AVG); nc_close(ncID_TEM_MAX); nc_close(ncID_TEM_MIN);
    nc_close(ncID_GEO); 
    printf("-------------------- xHM modelling: done!\n");
    return 1;
}


void malloc_error(
    int *data
)
{
    if (data == NULL)
    {
        printf("memory allocation failed!\n");
        exit(-3);
    }
    
}