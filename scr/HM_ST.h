/*
 * SUMMARY:      HM_ST.h
 * USAGE:        define data structures for the hydrological model level
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:
 * DESCRIP-END.
 *
 * COMMENTS:
 *
 *
 */

#ifndef HM_ST
#define HM_ST
#include "Constants.h"

typedef struct
{
    /* variables related to radiation */
    double Rs;        /* received shortwave radiation, [kJ/m2/h] */
    double L_sky;     /* received longwave radiation, [kJ/m2/h] */
    double Rno;       /* net radiation for the overstory, [kJ/m2/h] */
    double Rno_short; /* net shortwave radiation for the overstory, [kJ/m2/h] */
    double Rnu;       /* net radiation for the understory, [kJ/m2/h] */
    double Rnu_short; /* net shortwave radiation for the understory, [kJ/m2/h] */
    double Rns;       /* net radiation for ground/soil, [kJ/m2/h] */
} CELL_VAR_RADIA;

typedef struct
{
    /* variables in evapotranspiration processes */
    double Prec_throughfall; /* precipitation throughfall from overstory, [m] */
    double Prec_net;         /* net precipitation from understory into soil process, [m] */
    double Ep;               /* potential evapotranspiration, [m] */
    double EI_o;             /* actual evaporation, [m] */
    double ET_o;             /* actual transpiration, [m] */
    double EI_u;             /* actual evaporation, [m] */
    double ET_u;             /* actual transpiration, [m] */
    double ET_s;             /* soil evaporation, [m] */
    double Interception_o;   /* overstory interception water, [m] */
    double Interception_u;   /* understory interception water, [m] */
} CELL_VAR_ET;

typedef struct
{
    /* variables in soil water movement */
    double SM_Upper;             /* soil moisture: upper soil layer, FRAC */
    double SM_Lower;             /* soil moisture: lower soil layer, FRAC */
    double SW_Infiltration;      /* water infiltration from ground surface, [m] */
    double SW_Percolation_Upper; /* water percolation from upper soil layer, [m] */
    double SW_Percolation_Lower; /* water percolation from lower soil layer, [m] */
    double SW_SR_Infil;          /* surface runoff from excess-infiltration, [m] */
    double SW_SR_Satur;          /* surface runoff from saturation, [m] */

    double z;                    /* the water table of the grid cell, positive downward, [m] */
    int z_offset;                /* the water table from the reference height, 
                                    considering DEM difference with neighboring cells, positive downward, [m] */
    int z_offset_neighbor[8];    /* reference water table of 8 neighboring cells, [m] */
    double q[8];                 /* outflow from the cell to 8 directions, [m3/h] */
    int neighbor[8];             /* the status of the 8 neighbors, 1: valid, 0: nodata */
    double Qout;                 /* total outflow from this cell, [m3/h] */
    double Qin;                  /* total inflow to this cell, [m3/h] */
    double SW_rise_lower;        /* water volume suppied by a rising water table to the lower soil layer, [m] */
    double SW_rise_upper;        /* water volume suppied by a rising water table to the upper soil layer, [m] */
    double SW_rf;                /* water volume of return flow (generated when a rising water table reaches the ground surface), [m] */
} CELL_VAR_SOIL;


typedef struct 
{
    double k;           /* channel storage parameter, [1/h] */
    double Qc;          /* water flow from/into grid cell, [m3/h] */
    double Q;           /* discharge of the channel, [m3/h] */
    double V;           /* water volume of the channel, [m3] */
} CELL_VAR_STREAM;


typedef struct
{
    /******
     * see the explanation and units in OUTPUT_NAMELIST.txt
     * 1: yes, export the variable values into a separate nc file
     * default: 0
     */
    short Rs;
    short L_sky;
    short Rno;
    short Rnu;

    short Ep;
    short EI_o;
    short EI_u;
    short ET_o;
    short ET_u;
    short ET_s;
    short Interception_o;
    short Interception_u;
    short Prec_net;

    short SM_Upper;
    short SM_Lower;
    short SW_Run_Infil;
    short SW_Run_Satur;
    short SW_Infiltration;
    short SW_Percolation_Upper;
    short SW_Percolation_Lower;

    short SW_SUB_Qin;
    short SW_SUB_Qout;
    short SW_SUB_z;
    short SW_SUB_rise_upper;
    short SW_SUB_rise_lower;
    short SW_SUB_rf;

    short SW_SUB_Qc;
    short Q_Channel;

    short Qout_SF_Infil;
    short Qout_SF_Satur;
    short Qout_Sub;
    short Qout_outlet;
} OUT_NAME_LIST;

typedef struct
{
    /* weather data parameters */
    char FP_PRE[MAXCHAR];
    char FP_PRS[MAXCHAR];
    char FP_RHU[MAXCHAR];
    char FP_WIN[MAXCHAR];
    char FP_SSD[MAXCHAR];
    char FP_TEM_AVG[MAXCHAR];
    char FP_TEM_MAX[MAXCHAR];
    char FP_TEM_MIN[MAXCHAR];
    /* GEO info parameters */
    char FP_GEO[MAXCHAR];
    char FP_VEGLIB[MAXCHAR];
    char FP_SOILLIB[MAXCHAR];
    char FP_SOIL_HWSD_ID[MAXCHAR];
    double SOIL_D;
    double SOIL_d1;
    double SOIL_d2;
    double STREAM_D;
    double STREAM_W;
    double WIN_H;
    double ROUTE_CHANNEL_k;
    /* model setup parameters */
    int START_YEAR;
    int START_MONTH;
    int START_DAY;
    int START_HOUR;
    int STEP_TIME;
    int END_YEAR;
    int END_MONTH;
    int END_DAY;
    int END_HOUR;
    /* UH parameters */
    char FP_UH[MAXCHAR];
    char SURFACE_RUNOFF[30];
    double Velocity_avg;
    double Velocity_max;
    double Velocity_min;
    double b;
    double c;
    /* output parameters */
    char PATH_OUT[MAXCHAR];
    char FP_OUTNAMELIST[MAXCHAR];
} GLOBAL_PARA;

#endif
