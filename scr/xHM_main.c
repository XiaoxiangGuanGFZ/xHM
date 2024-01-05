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
#include "Initial_VAR.h"
#include "Evapotranspiration_ST.h"
#include "Evapotranspiration.h"
#include "Evapotranspiration_Energy.h"
#include "Evaporation_soil.h"
#include "GEO_ST.h"
#include "Resistance.h"
#include "Lookup_VegLib.h"



void main(int argc, char *argv[])
{
    /******************************************************************************
     *                            read parameter file
     */
    GPara_ET_ST GP;
    Import_GPara(*(++argv), &GP);
    printf("%s\n", GP.FP_WEATHER);

    /******************************************************************************
     *                              read GEO info
     */
    int ncID_GEO;
    ST_Header GEO_header;
    nc_open(GP.FP_GEO, NC_NOWRITE, &ncID_GEO);
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "ncols", &GEO_header.ncols);
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "nrows", &GEO_header.nrows);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "xllcorner", &GEO_header.xllcorner);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "yllcorner", &GEO_header.yllcorner);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "cellsize", &GEO_header.cellsize);
    printf("ncols: %d\nnrows: %d\nxllcorner: %.12f\nyllcorner: %.12f\ncellsize: %.12f\n",
           GEO_header.ncols, GEO_header.nrows, GEO_header.xllcorner, GEO_header.yllcorner, GEO_header.cellsize);

    /******************************************************************************
     *                      read gridded precipitation data
     */
    int ncID_PRE;
    int varID_PRE, varID_VEGTYPE, varID_VEGFRAC;
    int dimID_time;
    int time_steps_PRE;
    int *data_PRE;
    int *data_VEGTYPE;
    int *data_VEGFRAC;

    nc_open(GP.FP_PRE, NC_NOWRITE, &ncID_PRE);

    nc_inq_dimid(ncID_PRE, "time", &dimID_time);
    nc_inq_dimlen(ncID_PRE, dimID_time, &time_steps_PRE);
    printf("length of PRE observation: %d\n", time_steps_PRE);

    nc_inq_varid(ncID_PRE, "PRE", &varID_PRE);
    printf("varID_PRE: %d\n", varID_PRE);
    data_PRE = (int *)malloc(sizeof(int) * time_steps_PRE * GEO_header.ncols * GEO_header.nrows);
    nc_get_var_int(ncID_PRE, varID_PRE, data_PRE);

    data_VEGTYPE = (int *)malloc(sizeof(int) * time_steps_PRE * GEO_header.ncols * GEO_header.nrows);
    data_VEGFRAC = (int *)malloc(sizeof(int) * time_steps_PRE * GEO_header.ncols * GEO_header.nrows);
    nc_inq_varid(ncID_GEO, "VEGTYPE", &varID_VEGTYPE);
    nc_inq_varid(ncID_GEO, "VEGFRAC", &varID_VEGFRAC);
    nc_get_var_int(ncID_GEO, varID_VEGTYPE, data_VEGTYPE);
    nc_get_var_int(ncID_GEO, varID_VEGFRAC, data_VEGFRAC);

    /*******************************************************************************
     *                             read weather data
     */
    ST_Weather *data_weather;
    data_weather = (ST_Weather *)malloc(sizeof(ST_Weather) * MAXrows);
    int time_steps;
    Import_Weather(GP.FP_WEATHER, &data_weather, &time_steps);
    printf("steps in weather data: %d\n", time_steps);
    // for (size_t i = 0; i < time_steps; i++)
    // {
    //     printf("%.2f\n", (data_weather + i)->PRS);
    // }
    if (time_steps > MAXrows)
    {
        printf("number of rows in %s exceed the MAXrows %d, ", GP.FP_WEATHER, MAXrows);
        printf("you could increase the constant MAXrows in constants.h\n");
        exit(0);
    }

    int cell_index_row = 56; 
    int cell_index_col = 60;

    ST_VegLib db_veglib[11];
    Import_veglib(GP.FP_VEGLIB, db_veglib);
    
    double lat = 26.75;
    ST_CELL_VEG cell_veg;
    int cell_class;
    cell_class = *(data_VEGTYPE + cell_index_row * GEO_header.ncols + cell_index_col);
    cell_veg.CAN_FRAC = *(data_VEGFRAC + cell_index_row * GEO_header.ncols + cell_index_col) / 100;
    Lookup_VegLib_CELL(db_veglib, cell_class, &cell_veg);
    
    /**** radiation parameters *****/

    /**** vegetation parameters ****/
    double ws_obs_z = 10.0;/* the height of wind measurement */

    double SM = 0.5;      /* average soil moisture content */
    double SM_wp = 0.2;   /* the plant wilting point */
    double SM_free = 0.8; /* the moisture content above which soil conditions do not restrict transpiration. */
    double Soil_Fe = 0.005;

    /******
     * stage/intermediate variables 
    */
    double Rno;       /* net radiation for the overstory */
    double Rno_short; /* net shortwave radiation for the overstory */
    double Rnu;       /* net radiation for the understory */
    double Rnu_short; /* net shortwave radiation for the understory */
    double Rns;       /* net radiation for ground/soil */
    double Prec_throughfall; /* precipitation throughfall from overstory*/
    double Prec_net;         /* net precipitation from understory into soil process */
    double Ep;
    double EI_o;             /* actual evaporation, m */
    double ET_o;             /* actual transpiration, m */
    double EI_u;             /* actual evaporation, m */
    double ET_u;             /* actual transpiration, m */
    double ET_s;             /* soil evaporation, m */
    double Interception_o;   /* overstory interception water, m */
    double Interception_u;   /* understory interception water, m */
    
    Interception_o = 0.0; Interception_u = 0.0;
    /********************************************************************************
     *                       compute the evapotranspiration
     * */
    FILE *fp_wea;
    FILE *fp_ET;
    if ((fp_wea = fopen("D:/xHM/example_data/ET_weather.txt", "w")) == NULL)
    {
        printf("Failed in opening/creating file %s\n", "ET_weather.txt");
        exit(0);
    }
    if ((fp_ET = fopen("D:/xHM/example_data/ET_output.txt", "w")) == NULL)
    {
        printf("Failed in opening/creating file %s\n", "ET_output.txt");
        exit(0);
    }
    fprintf(
        fp_wea, "%8s%8s%8s%8s%8s%8s%8s%8s\n",
        "PRE", "TEM_AVG", "TEM_MAX", "TEM_MIN", "WIN", "SSD", "RHU", "PRS"
    );
    fprintf(fp_ET,
            "%8s%8s%8s%8s%8s%8s%8s\n",
            "Prec_net", "Ep", "EI_o", "ET_o", "EI_u", "ET_u", "ET_s");

    // int t = 23; // time index
    
    struct tm *ptm;
    /**** weather field ********/
    double cell_WIN;
    double cell_SSD;
    double cell_RHU;
    double cell_PRS;
    double cell_TEM_AVG;
    double cell_TEM_MAX;
    double cell_TEM_MIN;
    double cell_PRE;
    int year;
    int month;
    int day;
    for (size_t t = 0; t < 365; t++)
    {
        tm_increment(
            GP.START_YEAR,
            GP.START_MONTH,
            GP.START_DAY,
            GP.START_HOUR,
            &ptm,
            GP.STEP_TIME,
            t);
        year = ptm->tm_year + 1900;
        month = ptm->tm_mon + 1;
        day = ptm->tm_mday;
        // update the veg parameters 
        Lookup_VegLib_CELL_MON(
            db_veglib,
            cell_class,
            month,
            &cell_veg);

        // printf("%d-%d-%d\n", day, month, year);
        cell_PRE = (double)*(data_PRE + t * GEO_header.ncols * GEO_header.nrows + GEO_header.ncols * cell_index_row + cell_index_col) / 10;
        cell_PRE = cell_PRE / 1000; // unit: m
        cell_PRS = (data_weather + t)->PRS;
        cell_SSD = (data_weather + t)->SSD;
        cell_RHU = (data_weather + t)->RHU;
        cell_WIN = (data_weather + t)->WIN;
        cell_TEM_AVG = (data_weather + t)->TEM_AVG;
        cell_TEM_MAX = (data_weather + t)->TEM_MAX;
        cell_TEM_MIN = (data_weather + t)->TEM_MIN;
        fprintf(
            fp_wea, "%8.2f%8.2f%8.2f%8.2f%8.1f%8.0f%8.1f%8.1f\n",
            cell_PRE*1000, cell_TEM_AVG, cell_TEM_MAX, cell_TEM_MIN, cell_WIN, cell_SSD, cell_RHU, cell_PRS);
        ET_CELL(
            year, month, day, lat,
            cell_PRE, cell_TEM_AVG, cell_TEM_MIN, cell_TEM_MAX, cell_RHU, cell_PRS, cell_WIN,
            ws_obs_z, cell_SSD,
            &Rno, &Rno_short, &Rnu, &Rnu_short, &Rns,
            cell_veg.CAN_FRAC,
            cell_veg.Albedo_o, cell_veg.Albedo_u, ALBEDO_SOIL,
            cell_veg.LAI_o, cell_veg.LAI_u,
            cell_veg.Rpc, cell_veg.rs_min_o, RS_MAX,
            cell_veg.Rpc, cell_veg.rs_min_o, RS_MAX,

            cell_veg.CAN_RZ, cell_veg.CAN_H,
            cell_veg.d_o, cell_veg.z0_o,
            cell_veg.d_u, cell_veg.z0_u,

            SM, SM_wp, SM_free, Soil_Fe,

            &Prec_throughfall,
            &Prec_net,
            &Ep,
            &EI_o,
            &ET_o,
            &EI_u,
            &ET_u,
            &ET_s,
            &Interception_o,
            &Interception_u,
            cell_veg.Understory,
            GP.STEP_TIME);
        fprintf(fp_ET,
                "%10.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f\n",
                Prec_net * 1000, Ep * 1000 * GP.STEP_TIME, EI_o * 1000, ET_o * 1000, EI_u * 1000, ET_u * 1000, ET_s * 1000);
    }
    
    fclose(fp_wea);
    fclose(fp_ET);
    printf("-------------------- ET: done!\n");

}


