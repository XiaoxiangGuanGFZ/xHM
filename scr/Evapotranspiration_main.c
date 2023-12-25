
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <netcdf.h>
#include <time.h>
#include "constants.h"
#include "Calendar.h"
#include "Evapotranspiration_ST.h"
#include "Evapotranspiration.h"
#include "Evapotranspiration_Energy.h"
#include "Evaporation_soil.h"
#include "GEO_ST.h"
#include "Resistance.h"


void Import_GPara(
    char FP_GP[],
    GPara_ET_ST *GP);
void Import_Weather(
    char FP[],
    ST_Weather **data_weather,
    int *time_steps);

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
    int ncID;
    ST_Header GEO_header;
    nc_open(GP.FP_GEO, NC_NOWRITE, &ncID);
    nc_get_att_int(ncID, NC_GLOBAL, "ncols", &GEO_header.ncols);
    nc_get_att_int(ncID, NC_GLOBAL, "nrows", &GEO_header.nrows);
    nc_get_att_double(ncID, NC_GLOBAL, "xllcorner", &GEO_header.xllcorner);
    nc_get_att_double(ncID, NC_GLOBAL, "yllcorner", &GEO_header.yllcorner);
    nc_get_att_double(ncID, NC_GLOBAL, "cellsize", &GEO_header.cellsize);
    printf("ncols: %d\nnrows: %d\nxllcorner: %.12f\nyllcorner: %.12f\ncellsize: %.12f\n",
           GEO_header.ncols, GEO_header.nrows, GEO_header.xllcorner, GEO_header.yllcorner, GEO_header.cellsize);

    /******************************************************************************
     *                      read gridded precipitation data
     */
    int ncID_PRE;
    int varID_PRE;
    int dimID_time;
    int time_steps_PRE;
    int *data_PRE;

    nc_open(GP.FP_PRE, NC_NOWRITE, &ncID_PRE);

    nc_inq_dimid(ncID_PRE, "time", &dimID_time);
    nc_inq_dimlen(ncID_PRE, dimID_time, &time_steps_PRE);
    printf("length of PRE observation: %d\n", time_steps_PRE);

    nc_inq_varid(ncID_PRE, "PRE", &varID_PRE);
    printf("varID_PRE: %d\n", varID_PRE);
    data_PRE = (int *)malloc(sizeof(int) * time_steps_PRE * GEO_header.ncols * GEO_header.nrows);
    nc_get_var_int(ncID_PRE, varID_PRE, data_PRE);

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


    int step_time = GP.STEP_TIME;
    /**** radiation parameters *****/
    double lat = 26.75;
    double as = 0.25;
    double bs = 0.5;
    double Frac_canopy = 1.0;
    double Ref_o = 0.18;
    double Ref_u = 0.18;
    double Ref_s = 0.10;
    double LAI_o = 3.0;
    double LAI_u = 1.0;
    /**** vegetation parameters ****/
    double ws_obs_z = 10.0;/* the height of wind measurement */
    int Toggle_Understory = 1;
    double Canopy_zr = 20; /* reference height of canopy, m */
    double Canopy_h = 14;  /* height of canopy, m */
    double d_o = 9.2;      /* displacement height of canopy, m */
    double z0_o = 0.9;     /* the roughness of canopy, m */
    double d_u = 0.2;      /* displacement height of understory, m */
    double z0_u = 0.04;    /* roughness length of understory, m */

    double Rpc_o = 30;
    double Rpc_u = 30;
    double rs_min_o = 8.0;
    double rs_min_u = 1.2;
    double rs_max_o = 50;
    double rs_max_u = 50;
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
    int cell_index_row = 56; 
    int cell_index_col = 60;
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

            as, bs,
            &Rno, &Rno_short, &Rnu, &Rnu_short, &Rns,

            Frac_canopy,
            Ref_o, Ref_u, Ref_s,
            LAI_o, LAI_u,
            Rpc_o, rs_min_o, rs_max_o,
            Rpc_u, rs_min_u, rs_max_u,

            Canopy_zr, Canopy_h,
            d_o, z0_o,
            d_u, z0_u,

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
            Toggle_Understory,
            step_time);
        fprintf(fp_ET,
                "%10.2f%8.2f%8.2f%8.2f%8.2f%8.2f%8.2f\n",
                Prec_net * 1000, Ep * 1000 * GP.STEP_TIME, EI_o * 1000, ET_o * 1000, EI_u * 1000, ET_u * 1000, ET_s * 1000);
    }
    
    fclose(fp_wea);
    fclose(fp_ET);
    printf("-------------------- ET: done!\n");

}

void Import_GPara(
    char FP_GP[],
    GPara_ET_ST *GP)
{
    FILE *fp;
    if ((fp = fopen(FP_GP, "r")) == NULL)
    {
        printf("cannot open %s file\n", FP_GP);
        exit(0);
    }
    int i;
    char row[MAXCHAR];
    char S1[MAXCHAR];
    char S2[MAXCHAR];
    while (fgets(row, MAXCHAR, fp) != NULL)
    {
        if (row != NULL && strlen(row) > 1 && row[0] != '#')
        {
            for (i = 0; i < strlen(row); i++)
            {
                /* remove all the characters after # */
                if (row[i] == '#')
                {
                    row[i] = '\0'; // replace the '#' with '\0', end sign.
                }
            }
            if (sscanf(row, "%[^,],%s", S1, S2) == 2)
            {
                if (strcmp(S1, "FP_PRE") == 0)
                {
                    strcpy(GP->FP_PRE, S2);
                }
                else if (strcmp(S1, "FP_GEO") == 0)
                {
                    strcpy(GP->FP_GEO, S2);
                }
                else if (strcmp(S1, "FP_WEATHER") == 0)
                {
                    strcpy(GP->FP_WEATHER, S2);
                }
                else if (strcmp(S1, "START_YEAR") == 0)
                {
                    GP->START_YEAR = atoi(S2);
                }
                else if (strcmp(S1, "START_MONTH") == 0)
                {
                    GP->START_MONTH = atoi(S2);
                }
                else if (strcmp(S1, "START_DAY") == 0)
                {
                    GP->START_DAY = atoi(S2);
                }
                else if (strcmp(S1, "START_HOUR") == 0)
                {
                    GP->START_HOUR = atoi(S2);
                }
                else if (strcmp(S1, "STEP_TIME") == 0)
                {
                    GP->STEP_TIME = atoi(S2);
                }
                else
                {
                    printf("Unrecognized field!");
                    exit(0);
                }
            }
        }
    }
    fclose(fp);
}

void Import_Weather(
    char FP[],
    ST_Weather **data_weather,
    int *time_steps)
{
    FILE *fp;
    if ((fp = fopen(FP, "r")) == NULL)
    {
        printf("cannot open %s file\n", FP);
        exit(0);
    }
    int i;
    char row[MAXCHAR];
    i = 0;
    double PRS;
    double RHU;
    double SSD;
    double WIN;
    double TEM_AVG;
    double TEM_MAX;
    double TEM_MIN;
    while (fscanf(
               fp, "%lf,%lf,%lf,%lf,%lf,%lf,%lf",
               &PRS, &RHU, &SSD, &WIN, &TEM_AVG, &TEM_MAX, &TEM_MIN) == 7)
    {
        // printf("%.2f,%.0f,%.0f,%.1f,%.1f,%.1f,%.1f\n", PRS, RHU, SSD, WIN, TEM_AVG, TEM_MAX, TEM_MIN);

        (*data_weather + i)->PRS = PRS;
        (*data_weather + i)->RHU = RHU;
        (*data_weather + i)->SSD = SSD;
        (*data_weather + i)->WIN = WIN;
        (*data_weather + i)->TEM_AVG = TEM_AVG;
        (*data_weather + i)->TEM_MAX = TEM_MAX;
        (*data_weather + i)->TEM_MIN = TEM_MIN;
        i++;
    }
    *time_steps = i;
    fclose(fp);
}
