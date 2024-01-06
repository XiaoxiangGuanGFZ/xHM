
/*
 * SUMMARY:      Check_DATA.c
 * USAGE:        check the attributes in weather variables NetCDF files
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Jan-2024
 * DESCRIPTION:  the investigated attributes: ncols, nrows, cellsize_m, STEP_TIME
 *               and the model simulation period
 * DESCRIP-END.
 * FUNCTIONS:    Check_weather()
 * 
 * COMMENTS:
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <netcdf.h>

#include "NC_copy_global_att.h"
#include "Check_Data.h"

void Check_weather(
    int ncID_PRE, 
    int ncID_PRS, 
    int ncID_RHU, 
    int ncID_SSD, 
    int ncID_WIN, 
    int ncID_TEM_AVG, 
    int ncID_TEM_MAX, 
    int ncID_TEM_MIN,
    time_t START_TIME,
    time_t END_TIME,
    int STEP_TIME
)
{
    /************* check the variables  **************/
    int status_nc;
    int varID;
    status_nc = nc_inq_varid(ncID_PRE, "PRE", &varID); handle_error(status_nc, "PRE"); 
    status_nc = nc_inq_varid(ncID_PRS, "PRS", &varID); handle_error(status_nc, "PRS"); 
    status_nc = nc_inq_varid(ncID_RHU, "RHU", &varID); handle_error(status_nc, "RHU"); 
    status_nc = nc_inq_varid(ncID_SSD, "SSD", &varID); handle_error(status_nc, "SSD"); 
    status_nc = nc_inq_varid(ncID_WIN, "WIN", &varID); handle_error(status_nc, "WIN"); 
    status_nc = nc_inq_varid(ncID_TEM_AVG, "TEM_AVG", &varID); handle_error(status_nc, "TEM_AVG"); 
    status_nc = nc_inq_varid(ncID_TEM_MAX, "TEM_MAX", &varID); handle_error(status_nc, "TEM_MAX"); 
    status_nc = nc_inq_varid(ncID_TEM_MIN, "TEM_MIN", &varID); handle_error(status_nc, "TEM_MIN"); 

    /*************** check the attribute values ************/ 
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
        printf("Error: weather data share different global attribute: ncols\n");
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
        printf("Error: weather data share different global attribute: nrows\n");
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
        printf("Error: weather data share different global attribute: cellsize_m\n");
        exit(-2);
    }
    // STEP_TIME
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
        printf("Error: weather data share different variable attribute: STEP_TIME\n");
        exit(-2);
    }

    /***************** model running period *****************/ 
    if (START_TIME > END_TIME)
    {
        printf("Error: invalid start time or end time for the model running. Please check the global parameter file!\n");
        exit(-2);
    }
    
    int dimID_time;
    long t_PRE, t_PRS, t_SSD, t_RHU, t_WIN, t_TEM_AVG, t_TEM_MAX, t_TEM_MIN;
    int time_steps_PRE, time_steps_PRS, time_steps_RHU, time_steps_SSD, time_steps_WIN, time_steps_TEM_AVG, time_steps_TEM_MAX, time_steps_TEM_MIN;
    size_t index = 0;
    int t_unit_s;
    t_unit_s = STEP_TIME * 3600;
    nc_inq_varid(ncID_PRE, "time", &varID); nc_get_var1_long(ncID_PRE, varID, &index, &t_PRE); 
    nc_inq_dimid(ncID_PRE, "time", &dimID_time); nc_inq_dimlen(ncID_PRE, dimID_time, &time_steps_PRE);

    nc_inq_varid(ncID_PRS, "time", &varID); nc_get_var1_long(ncID_PRS, varID, &index, &t_PRS);
    nc_inq_dimid(ncID_PRS, "time", &dimID_time); nc_inq_dimlen(ncID_PRS, dimID_time, &time_steps_PRS);

    nc_inq_varid(ncID_SSD, "time", &varID); nc_get_var1_long(ncID_SSD, varID, &index, &t_SSD);
    nc_inq_dimid(ncID_SSD, "time", &dimID_time); nc_inq_dimlen(ncID_SSD, dimID_time, &time_steps_SSD);

    nc_inq_varid(ncID_RHU, "time", &varID); nc_get_var1_long(ncID_RHU, varID, &index, &t_RHU);
    nc_inq_dimid(ncID_RHU, "time", &dimID_time); nc_inq_dimlen(ncID_RHU, dimID_time, &time_steps_RHU);

    nc_inq_varid(ncID_WIN, "time", &varID); nc_get_var1_long(ncID_WIN, varID, &index, &t_WIN);
    nc_inq_dimid(ncID_WIN, "time", &dimID_time); nc_inq_dimlen(ncID_WIN, dimID_time, &time_steps_WIN);

    nc_inq_varid(ncID_TEM_AVG, "time", &varID); nc_get_var1_long(ncID_TEM_AVG, varID, &index, &t_TEM_AVG);
    nc_inq_dimid(ncID_TEM_AVG, "time", &dimID_time); nc_inq_dimlen(ncID_TEM_AVG, dimID_time, &time_steps_TEM_AVG);

    nc_inq_varid(ncID_TEM_MAX, "time", &varID); nc_get_var1_long(ncID_TEM_MAX, varID, &index, &t_TEM_MAX);
    nc_inq_dimid(ncID_TEM_MAX, "time", &dimID_time); nc_inq_dimlen(ncID_TEM_MAX, dimID_time, &time_steps_TEM_MAX);

    nc_inq_varid(ncID_TEM_MIN, "time", &varID); nc_get_var1_long(ncID_TEM_MIN, varID, &index, &t_TEM_MIN); 
    nc_inq_dimid(ncID_TEM_MIN, "time", &dimID_time); nc_inq_dimlen(ncID_TEM_MIN, dimID_time, &time_steps_TEM_MIN);
    // printf("time_steps_PRE: %d\n", time_steps_PRE);
    // printf("START_TIME: %ld\nEND_TIME: %ld\n", START_TIME, END_TIME);
    // printf("For PRE:\nSTART_TIME: %ld\n", t_PRE);
    if (!(START_TIME >= t_PRE && START_TIME <= (t_PRE + time_steps_PRE * t_unit_s) &&
          END_TIME >= t_PRE && END_TIME <= (t_PRE + time_steps_PRE * t_unit_s)))
    {
        printf("Error: no overlapping period between the running period and PRE data! \n");
        exit(-2);
    }

    if (!(START_TIME >= t_PRS && START_TIME <= (t_PRS + time_steps_PRS * t_unit_s) &&
          END_TIME >= t_PRS && END_TIME <= (t_PRS + time_steps_PRS * t_unit_s)))
    {
        printf("Error: no overlapping period between the running period and PRS data! \n");
        exit(-2);
    }

    if (!(START_TIME >= t_SSD && START_TIME <= (t_SSD + time_steps_SSD * t_unit_s) &&
          END_TIME >= t_SSD && END_TIME <= (t_SSD + time_steps_SSD * t_unit_s)))
    {
        printf("Error: no overlapping period between the running period and SSD data! \n");
        exit(-2);
    }

    if (!(START_TIME >= t_RHU && START_TIME <= (t_RHU + time_steps_RHU * t_unit_s) &&
          END_TIME >= t_RHU && END_TIME <= (t_RHU + time_steps_RHU * t_unit_s)))
    {
        printf("Error: no overlapping period between the running period and RHU data! \n");
        exit(-2);
    }

    if (!(START_TIME >= t_WIN && START_TIME <= (t_WIN + time_steps_WIN * t_unit_s) &&
          END_TIME >= t_WIN && END_TIME <= (t_WIN + time_steps_WIN * t_unit_s)))
    {
        printf("Error: no overlapping period between the running period and WIN data! \n");
        exit(-2);
    }

    if (!(START_TIME >= t_TEM_AVG && START_TIME <= (t_TEM_AVG + time_steps_TEM_AVG * t_unit_s) &&
          END_TIME >= t_TEM_AVG && END_TIME <= (t_TEM_AVG + time_steps_TEM_AVG * t_unit_s)))
    {
        printf("Error: no overlapping period between the running period and TEM_AVG data! \n");
        exit(-2);
    }

    if (!(START_TIME >= t_TEM_MAX && START_TIME <= (t_TEM_MAX + time_steps_WIN * t_unit_s) &&
          END_TIME >= t_TEM_MAX && END_TIME <= (t_TEM_MAX + time_steps_WIN * t_unit_s)))
    {
        printf("Error: no overlapping period between the running period and TEM_MAX data! \n");
        exit(-2);
    }

    if (!(START_TIME >= t_TEM_MIN && START_TIME <= (t_TEM_MIN + time_steps_TEM_MIN * t_unit_s) &&
          END_TIME >= t_TEM_MIN && END_TIME <= (t_TEM_MIN + time_steps_TEM_MIN * t_unit_s)))
    {
        printf("Error: no overlapping period between the running period and TEM_MIN data! \n");
        exit(-2);
    }
}

void Check_GEO(
    int ncID_GEO
)
{
    int status_nc;
    int varID;
    status_nc = nc_inq_varid(ncID_GEO, "DEM", &varID); handle_error(status_nc, "GEO.nc: DEM"); 
    status_nc = nc_inq_varid(ncID_GEO, "FDR", &varID); handle_error(status_nc, "GEO.nc: FDR"); 
    status_nc = nc_inq_varid(ncID_GEO, "FAC", &varID); handle_error(status_nc, "GEO.nc: FAC"); 
    status_nc = nc_inq_varid(ncID_GEO, "STR", &varID); handle_error(status_nc, "GEO.nc: STR"); 
    status_nc = nc_inq_varid(ncID_GEO, "OUTLET", &varID); handle_error(status_nc, "GEO.nc: OUTLET"); 
    status_nc = nc_inq_varid(ncID_GEO, "VEGTYPE", &varID); handle_error(status_nc, "GEO.nc: VEGTYPE"); 
    status_nc = nc_inq_varid(ncID_GEO, "VEGFRAC", &varID); handle_error(status_nc, "GEO.nc: VEGFRAC"); 
    status_nc = nc_inq_varid(ncID_GEO, "SOILTYPE", &varID); handle_error(status_nc, "GEO.nc: SOILTYPE"); 
}
// int main(int argc, char const *argv[])
// {
//     /* code */
//     return 0;
// }
