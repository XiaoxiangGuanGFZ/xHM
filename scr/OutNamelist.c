
/*
 * SUMMARY:      OutNamelist.c
 * USAGE:        output variables
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  read the outnamelist file to inform the program
 *               of the variables in simulation which should be saved and written 
 *               to NetCDF files
 * DESCRIP-END.
 * FUNCTIONS:    Import_Outnamelist(); Initialize_Outnamelist(); 
 *               malloc_Outnamelist(); Write2NC_Outnamelist()
 * 
 * COMMENTS:
 * - read the outnamelist.txt file
 * - initialize the outnamelist structure
 * - allocate memory to the specified output variables
 * - write the results into NetCDF files
 *
 * REFERENCES:
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Constants.h"
#include "HM_ST.h"
#include "OutNamelist.h"
#include "NetCDF_IO_geo.h"
void Import_Outnamelist(
    char FP[],
    OUT_NAME_LIST *outnl
)
{
    FILE *fp;
    if ((fp = fopen(FP, "r")) == NULL)
    {
        printf("cannot open file %s\n", FP);
        exit(0);
    }
    int i;  // character indexing in a string,
    int j = 0;
    char row[MAXCHAR];
    char S1[MAXCHAR];
    char S2[MAXCHAR];
    while (fgets(row, MAXCHAR, fp) != NULL)
    {
        j++;
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
                if (strcmp(S1, "EI_o") == 0)
                {
                    outnl->EI_o = atoi(S2);
                }
                else if (strcmp(S1, "EI_u") == 0)
                {
                    outnl->EI_u = atoi(S2);
                }
                else if (strcmp(S1, "ET_o") == 0)
                {
                    outnl->ET_o = atoi(S2);
                } 
                else if (strcmp(S1, "ET_u") == 0)
                {
                    outnl->ET_u = atoi(S2);
                }
                else if (strcmp(S1, "ET_s") == 0)
                {
                    outnl->ET_s = atoi(S2);
                }
                else if (strcmp(S1, "Ep") == 0)
                {
                    outnl->Ep = atoi(S2);
                }
                else if (strcmp(S1, "Interception_u") == 0)
                {
                    outnl->Interception_u = atoi(S2);
                }
                else if (strcmp(S1, "Interception_o") == 0)
                {
                    outnl->Interception_o = atoi(S2);
                }
                else if (strcmp(S1, "Prec_net") == 0)
                {
                    outnl->Prec_net = atoi(S2);
                }
                else if (strcmp(S1, "Rs") == 0)
                {
                    outnl->Rs = atoi(S2);
                }
                else if (strcmp(S1, "Rno") == 0)
                {
                    outnl->Rno = atoi(S2);
                }
                else if (strcmp(S1, "Rnu") == 0)
                {
                    outnl->Rnu = atoi(S2);
                }
                else if (strcmp(S1, "L_sky") == 0)
                {
                    outnl->L_sky = atoi(S2);
                } 
                else if (strcmp(S1, "SM_Lower") == 0)
                {
                    outnl->SM_Lower = atoi(S2);
                } 
                else if (strcmp(S1, "SM_Upper") == 0)
                {
                    outnl->SM_Upper = atoi(S2);
                }
                else if (strcmp(S1, "SW_Infiltration") == 0)
                {
                    outnl->SW_Infiltration = atoi(S2);
                }
                else if (strcmp(S1, "SW_Percolation_Lower") == 0)
                {
                    outnl->SW_Percolation_Lower = atoi(S2);
                }
                else if (strcmp(S1, "SW_Percolation_Upper") == 0)
                {
                    outnl->SW_Percolation_Upper = atoi(S2);
                }
                else if (strcmp(S1, "SW_Run_Infil") == 0)
                {
                    outnl->SW_Run_Infil = atoi(S2);
                }
                else if (strcmp(S1, "SW_Run_Satur") == 0)
                {
                    outnl->SW_Run_Satur = atoi(S2);
                }
                else if (strcmp(S1, "SW_SUB_Qin") == 0)
                {
                    outnl->SW_SUB_Qin = atoi(S2);
                }
                else if (strcmp(S1, "SW_SUB_Qout") == 0)
                {
                    outnl->SW_SUB_Qout = atoi(S2);
                }
                else if (strcmp(S1, "SW_SUB_z") == 0)
                {
                    outnl->SW_SUB_z = atoi(S2);
                }
                else if (strcmp(S1, "SW_SUB_rise_upper") == 0)
                {
                    outnl->SW_SUB_rise_upper = atoi(S2);
                }
                else if (strcmp(S1, "SW_SUB_rise_lower") == 0)
                {
                    outnl->SW_SUB_rise_lower = atoi(S2);
                }
                else if (strcmp(S1, "SW_SUB_rf") == 0)
                {
                    outnl->SW_SUB_rf = atoi(S2);
                }
                else if (strcmp(S1, "SW_SUB_Qc") == 0)
                {
                    outnl->SW_SUB_Qc = atoi(S2);
                }
                else if (strcmp(S1, "Q_Channel") == 0)
                {
                    outnl->Q_Channel = atoi(S2);
                }
                else if (strcmp(S1, "Qout_SF_Infil") == 0)
                {
                    outnl->Qout_SF_Infil = atoi(S2);
                }
                else if (strcmp(S1, "Qout_SF_Satur") == 0)
                {
                    outnl->Qout_SF_Satur = atoi(S2);
                }
                else if (strcmp(S1, "Qout_Sub") == 0)
                {
                    outnl->Qout_Sub = atoi(S2);
                }
                else if (strcmp(S1, "Qout_outlet") == 0)
                {
                    outnl->Qout_outlet = atoi(S2);
                }
                else
                {
                    printf("Unrecognized field in row %d in OUTPUT_NAMELIST.txt: %s\n", j, row);
                    exit(0);
                }
            }
        }
    }
    fclose(fp);
}

void Initialize_Outnamelist(
    OUT_NAME_LIST *outnl
)
{

    for (size_t i = 0; i < sizeof(OUT_NAME_LIST) / sizeof(short); i++)
    {
        short *currentMember = ((short*)outnl) + i;
        *currentMember = 0;
    }
}

void malloc_Outnamelist(
    OUT_NAME_LIST outnl,
    int cell_counts_total,
    int time_steps_run,
    int **out_Rs,
    int **out_L_sky,
    int **out_Rno,
    int **out_Rnu,
    int **out_Ep,
    int **out_EI_o,
    int **out_EI_u,
    int **out_ET_o,
    int **out_ET_u,
    int **out_ET_s,
    int **out_Interception_o,
    int **out_Interception_u,
    int **out_Prec_net,
    int **out_SM_Upper,
    int **out_SM_Lower,
    int **out_SW_Percolation_Upper,
    int **out_SW_Percolation_Lower,
    int **out_SW_Infiltration,
    int **out_SW_Run_Infil,
    int **out_SW_Run_Satur,
    int **out_SW_SUB_Qin, 
    int **out_SW_SUB_Qout, 
    int **out_SW_SUB_z, 
    int **out_SW_SUB_rise_upper, 
    int **out_SW_SUB_rise_lower, 
    int **out_SW_SUB_rf,
    int **out_SW_SUB_Qc,
    int **out_Q_Channel
)
{
    long size;
    size = time_steps_run * cell_counts_total;
    // radiation vars
    if (outnl.Rs == 1)
    {
        *out_Rs = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_Rs, "Rs");
    }
    if (outnl.L_sky == 1)
    {
        *out_L_sky = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_L_sky, "L_sky");
    }
    if (outnl.Rno == 1)
    {
        *out_Rno = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_Rno, "Rno");
    }
    if (outnl.Rnu == 1)
    {
        *out_Rnu = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_Rnu, "Rnu");
    }
    // ET variables
    if (outnl.Ep == 1)
    {
        *out_Ep = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_Ep, "Ep");
    }
    if (outnl.EI_o == 1)
    {
        *out_EI_o = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_EI_o, "EI_o");
    }
    if (outnl.EI_u == 1)
    {
        *out_EI_u = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_EI_u, "EI_u");
    }
    if (outnl.ET_o == 1)
    {
        *out_ET_o = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_ET_o, "ET_o");
    }
    if (outnl.ET_u == 1)
    {
        *out_ET_u = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_ET_u, "ET_u");
    }
    if (outnl.ET_s == 1)
    {
        *out_ET_s = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_ET_s, "ET_s");
    }
    if (outnl.Interception_o == 1)
    {
        *out_Interception_o = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_Interception_o, "Interception_o");
    }
    if (outnl.Interception_u == 1)
    {
        *out_Interception_u = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_Interception_u, "Interception_u");
    }
    if (outnl.Prec_net == 1)
    {
        *out_Prec_net = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_Prec_net, "Prec_net");
    }
    // soil variables
    if (outnl.SM_Lower == 1)
    {
        *out_SM_Lower = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SM_Lower, "SM_Lower");
    }
    if (outnl.SM_Upper == 1)
    {
        *out_SM_Upper = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SM_Upper, "SM_Upper");
    }
    if (outnl.SW_Infiltration == 1)
    {
        *out_SW_Infiltration = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SW_Infiltration, "SW_Infiltration");
    }
    if (outnl.SW_Percolation_Upper == 1)
    {
        *out_SW_Percolation_Upper = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SW_Percolation_Upper, "SW_Percolation_Upper");
    }
    if (outnl.SW_Percolation_Lower == 1)
    {
        *out_SW_Percolation_Lower = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SW_Percolation_Lower, "SW_Percolation_Lower");
    }
    // if (outnl.SW_Run_Infil == 1)
    // {
    //     *out_SW_Run_Infil = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SW_Run_Infil, "SW_Run_Infil");
    // }
    // if (outnl.SW_Run_Satur == 1)
    // {
    //     *out_SW_Run_Satur = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SW_Run_Satur, "SW_Run_Satur");
    // }

    if (outnl.SW_SUB_Qc == 1)
    {
        *out_SW_SUB_Qc = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SW_SUB_Qc, "SW_SUB_Qc");
    }
    if (outnl.Q_Channel == 1)
    {
        *out_Q_Channel = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_Q_Channel, "Q_Channel");
    }

    if (outnl.SW_SUB_Qin == 1)
    {
        *out_SW_SUB_Qin = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SW_SUB_Qin, "SW_SUB_Qin");
    }
    if (outnl.SW_SUB_Qout == 1)
    {
        *out_SW_SUB_Qout = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SW_SUB_Qout, "SW_SUB_Qout");
    }
    if (outnl.SW_SUB_z == 1)
    {
        *out_SW_SUB_z = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SW_SUB_z, "SW_SUB_z");
    }
    if (outnl.SW_SUB_rise_lower == 1)
    {
        *out_SW_SUB_rise_lower = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SW_SUB_rise_lower, "SW_SUB_rise_lower");
    }
    if (outnl.SW_SUB_rise_upper == 1)
    {
        *out_SW_SUB_rise_upper = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SW_SUB_rise_upper, "SW_SUB_rise_upper");
    }
    if (outnl.SW_SUB_rf == 1)
    {
        *out_SW_SUB_rf = (int *)malloc(sizeof(int) * size); malloc_memory_error(*out_SW_SUB_rf, "SW_SUB_rf");
    }
}

void malloc_memory_error(
    int *data,
    char var[]
)
{
    if (data == NULL)
    {
        printf("memory allocation failed for variable %s!\n", var);
        exit(-3);
    }
}
void Write2NC_Outnamelist(
    OUT_NAME_LIST outnl,
    int time_steps_run,
    int **out_Rs,
    int **out_L_sky,
    int **out_Rno,
    int **out_Rnu,
    int **out_Ep,
    int **out_EI_o,
    int **out_EI_u,
    int **out_ET_o,
    int **out_ET_u,
    int **out_ET_s,
    int **out_Interception_o,
    int **out_Interception_u,
    int **out_Prec_net,
    int **out_SM_Upper,
    int **out_SM_Lower,
    int **out_SW_Percolation_Upper,
    int **out_SW_Percolation_Lower,
    int **out_SW_Infiltration,
    int **out_SW_Run_Infil,
    int **out_SW_Run_Satur,
    int **out_SW_SUB_Qin, 
    int **out_SW_SUB_Qout, 
    int **out_SW_SUB_z, 
    int **out_SW_SUB_rise_upper, 
    int **out_SW_SUB_rise_lower, 
    int **out_SW_SUB_rf,
    int **out_SW_SUB_Qc,
    int **out_Q_Channel,
    GLOBAL_PARA GP
)
{
    char FP_OUT_VAR[MAXCHAR];
    if (outnl.Rs == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Rs.nc");
        Write2NC("Rs", "kJ/m2/h", "canopy received shortwave radiation",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_Rs,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.L_sky == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "L_sky.nc");
        Write2NC("L_sky", "kJ/m2/h", "canopy received longwave radiation",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_L_sky,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.Rno == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Rno.nc");
        Write2NC("Rno", "kJ/m2/h", "canopy received net radiation",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_Rno,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.Rnu == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Rnu.nc");
        Write2NC("Rnu", "kJ/m2/h", "understory received net radiation",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_Rnu,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.Ep == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Ep.nc");
        Write2NC("Ep", "mm", "potential evapotranspiration",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_Ep,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.EI_o == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "EI_o.nc");
        Write2NC("EI_o", "mm", "overstory evaporation",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_EI_o,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.EI_u == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "EI_u.nc");
        Write2NC("EI_u", "mm", "understory evaporation",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_EI_u,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.ET_o == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "ET_o.nc");
        Write2NC("ET_o", "mm", "overstory transpiration",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_ET_o,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.ET_u == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "ET_u.nc");
        Write2NC("ET_u", "mm", "understory transpiration",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_ET_u,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.ET_s == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "ET_s.nc");
        Write2NC("ET_s", "mm", "soil evaporation",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_ET_s,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.Prec_net == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Prec_net.nc");
        Write2NC("Prec_net", "mm", "net precipitation",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_Prec_net,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    // soil variables
    if (outnl.SM_Upper == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SM_Upper.nc");
        Write2NC("SM_Upper", "FRAC", "soil moisture of upper soil layer",
                 0.01, GP.FP_GEO, FP_OUT_VAR, out_SM_Upper,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.SM_Lower == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SM_Lower.nc");
        Write2NC("SM_Lower", "FRAC", "soil moisture of lower soil layer",
                 0.01, GP.FP_GEO, FP_OUT_VAR, out_SM_Lower,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.SW_Infiltration == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_Infiltration.nc");
        Write2NC("SW_Infiltration", "mm", "infiltration water",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_SW_Infiltration,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.SW_Percolation_Upper == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_Percolation_Upper.nc");
        Write2NC("SW_Percolation_Upper", "mm", "upper soil layer water percolation",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_SW_Percolation_Upper,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.SW_Percolation_Lower == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_Percolation_Lower.nc");
        Write2NC("SW_Percolation_Lower", "mm", "lower soil layer water percolation",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_SW_Percolation_Lower,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.SW_Run_Infil == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_Run_Infil.nc");
        Write2NC("SW_Run_Infil", "mm", "surface runoff from infiltration-excess",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_SW_Run_Infil,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.SW_Run_Satur == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_Run_Satur.nc");
        Write2NC("SW_Run_Satur", "mm", "surface runoff from saturation-excess",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_SW_Run_Satur,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    // subsurface 
    if (outnl.SW_SUB_Qin == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_Qin.nc");
        Write2NC("SW_SUB_Qin", "mm", "subsurface inflow to the grid cell",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_SW_SUB_Qin,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.SW_SUB_Qout == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_Qout.nc");
        Write2NC("SW_SUB_Qout", "mm", "subsurface outflow from the grid cell",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_SW_SUB_Qout,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.SW_SUB_z == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_z.nc");
        Write2NC("SW_SUB_z", "m", "subsurface water table",
                 0.01, GP.FP_GEO, FP_OUT_VAR, out_SW_SUB_z,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.SW_SUB_rise_lower == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_rise_lower.nc");
        Write2NC("SW_SUB_rise_lower", "mm", "water supplied by rising water table to lower soil layer",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_SW_SUB_rise_lower,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.SW_SUB_rise_upper == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_rise_upper.nc");
        Write2NC("SW_SUB_rise_upper", "mm", "water supplied by rising water table to upper soil layer",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_SW_SUB_rise_upper,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.SW_SUB_rf == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_rf.nc");
        Write2NC("SW_SUB_rf", "mm", "water volume of returnflow",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_SW_SUB_rf,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }

    if (outnl.SW_SUB_Qc == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_Qc.nc");
        Write2NC("SW_SUB_Qc", "mm", "lateral water into river channel",
                 0.1, GP.FP_GEO, FP_OUT_VAR, out_SW_SUB_Qc,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
    if (outnl.Q_Channel == 1)
    {
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Q_Channel.nc");
        Write2NC("Q_Channel", "m3/s", "subsurface-induced discharge in river channels",
                 0.001, GP.FP_GEO, FP_OUT_VAR, out_Q_Channel,
                 GP.STEP_TIME, time_steps_run, GP.START_YEAR, GP.START_MONTH, GP.START_DAY, GP.START_HOUR);
    }
}
// int main(int argc, char const *argv[])
// {
//     /* code */
//     OUT_NAME_LIST outnl;
//     Initialize_Outnamelist(&outnl);
//     printf("Ep: %d\n", outnl.Ep);
//     printf("Rs: %d\n", outnl.Rs);
//     printf("SW_Run_Infil: %d\n", outnl.SW_Run_Infil);

//     Import_Outnamelist(
//         "D:/xHM/example_data/OUTPUT_NAMELIST.txt",
//         &outnl);
//     printf("Ep: %d\n", outnl.Ep);
//     printf("Rs: %d\n", outnl.Rs);
//     printf("SW_Run_Infil: %d\n", outnl.SW_Run_Infil);
//     return 0;
// }
