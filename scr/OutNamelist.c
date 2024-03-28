
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
#include <time.h>
#include <netcdf.h>
#include "Constants.h"
#include "GEO_ST.h"
#include "HM_ST.h"
#include "OutNamelist.h"
#include "NC_copy_global_att.h"
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
    OUT_NAME_LIST *outnl_ncid,
    int cell_counts_total,
    int time_steps_run,
    int **data_DEM,
    ST_Header HD,
    GLOBAL_PARA GP,
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
    size = 1 * cell_counts_total; // size = time_steps_run * cell_counts_total;
    char FP_OUT_VAR[MAXCHAR];
    // radiation vars
    if (outnl.Rs == 1)
    {
        *out_Rs = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_Rs, "Rs");
        OUTVAR_nc_initial(data_DEM, out_Rs, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Rs.nc");
        outnl_ncid->Rs = OUTVAR_nc_create("Rs", "kJ/m2/h", "canopy received shortwave radiation", 0.1,
                                         FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.L_sky == 1)
    {
        *out_L_sky = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_L_sky, "L_sky");
        OUTVAR_nc_initial(data_DEM, out_L_sky, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "L_sky.nc");
        outnl_ncid->L_sky = OUTVAR_nc_create("L_sky", "kJ/m2/h", "canopy received longwave radiation", 0.1,
                                            FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.Rno == 1)
    {
        *out_Rno = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_Rno, "Rno");
        OUTVAR_nc_initial(data_DEM, out_Rno, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Rno.nc");
        outnl_ncid->Rno = OUTVAR_nc_create("Rno", "kJ/m2/h", "canopy received net radiation", 0.1,
                                          FP_OUT_VAR, time_steps_run, GP);
    }

    if (outnl.Rnu == 1)
    {
        *out_Rnu = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_Rnu, "Rnu");
        OUTVAR_nc_initial(data_DEM, out_Rnu, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Rnu.nc");
        outnl_ncid->Rnu = OUTVAR_nc_create("Rnu", "kJ/m2/h", "understory received net radiation", 0.1,
                                          FP_OUT_VAR, time_steps_run, GP);
    }
    // ET variables
    if (outnl.Ep == 1)
    {
        *out_Ep = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_Ep, "Ep");
        OUTVAR_nc_initial(data_DEM, out_Ep, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Ep.nc");
        outnl_ncid->Ep = OUTVAR_nc_create("Ep", "mm", "potential evapotranspiration", 0.1,
                                         FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.EI_o == 1)
    {
        *out_EI_o = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_EI_o, "EI_o");
        OUTVAR_nc_initial(data_DEM, out_EI_o, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "EI_o.nc");
        outnl_ncid->EI_o = OUTVAR_nc_create("EI_o", "mm", "overstory evaporation", 0.1,
                                           FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.EI_u == 1)
    {
        *out_EI_u = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_EI_u, "EI_u");
        OUTVAR_nc_initial(data_DEM, out_EI_u, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "EI_u.nc");
        outnl_ncid->EI_u = OUTVAR_nc_create("EI_u", "mm", "understory evaporation", 0.1,
                                           FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.ET_o == 1)
    {
        *out_ET_o = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_ET_o, "ET_o");
        OUTVAR_nc_initial(data_DEM, out_ET_o, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "ET_o.nc");
        outnl_ncid->ET_o = OUTVAR_nc_create("ET_o", "mm", "overstory transpiration", 0.1,
                                           FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.ET_u == 1)
    {
        *out_ET_u = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_ET_u, "ET_u");
        OUTVAR_nc_initial(data_DEM, out_ET_u, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "ET_u.nc");
        outnl_ncid->ET_u = OUTVAR_nc_create("ET_u", "mm", "understory transpiration", 0.1,
                                           FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.ET_s == 1)
    {
        *out_ET_s = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_ET_s, "ET_s");
        OUTVAR_nc_initial(data_DEM, out_ET_s, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "ET_s.nc");
        outnl_ncid->ET_s = OUTVAR_nc_create("ET_s", "mm", "soil evaporation", 0.1,
                                           FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.Interception_o == 1)
    {
        *out_Interception_o = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_Interception_o, "Interception_o");
        OUTVAR_nc_initial(data_DEM, out_Interception_o, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Interception_o.nc");
        outnl_ncid->Interception_o = OUTVAR_nc_create("Interception_o", "mm", "intercepted water by overstory", 0.1,
                                                     FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.Interception_u == 1)
    {
        *out_Interception_u = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_Interception_u, "Interception_u");
        OUTVAR_nc_initial(data_DEM, out_Interception_u, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Interception_u.nc");
        outnl_ncid->Interception_u = OUTVAR_nc_create("Interception_u", "mm", "intercepted water by understory", 0.1,
                                                     FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.Prec_net == 1)
    {
        *out_Prec_net = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_Prec_net, "Prec_net");
        OUTVAR_nc_initial(data_DEM, out_Prec_net, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Prec_net.nc");
        outnl_ncid->Prec_net = OUTVAR_nc_create("Prec_net", "mm", "net precipitation", 0.1,
                                               FP_OUT_VAR, time_steps_run, GP);
    }
    // soil variables
    if (outnl.SM_Lower == 1)
    {
        *out_SM_Lower = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_SM_Lower, "SM_Lower");
        OUTVAR_nc_initial(data_DEM, out_SM_Lower, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SM_Lower.nc");
        outnl_ncid->SM_Lower = OUTVAR_nc_create("SM_Lower", "FRAC", "soil moisture of lower soil layer", 0.01,
                                               FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.SM_Upper == 1)
    {
        *out_SM_Upper = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_SM_Upper, "SM_Upper");
        OUTVAR_nc_initial(data_DEM, out_SM_Upper, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SM_Upper.nc");
        outnl_ncid->SM_Upper = OUTVAR_nc_create("SM_Upper", "FRAC", "soil moisture of upper soil layer", 0.01,
                                               FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.SW_Infiltration == 1)
    {
        *out_SW_Infiltration = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_SW_Infiltration, "SW_Infiltration");
        OUTVAR_nc_initial(data_DEM, out_SW_Infiltration, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_Infiltration.nc");
        outnl_ncid->SW_Infiltration = OUTVAR_nc_create("SW_Infiltration", "mm", "infiltration water", 0.1,
                                                      FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.SW_Percolation_Upper == 1)
    {
        *out_SW_Percolation_Upper = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_SW_Percolation_Upper, "SW_Percolation_Upper");
        OUTVAR_nc_initial(data_DEM, out_SW_Percolation_Upper, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_Percolation_Upper.nc");
        outnl_ncid->SW_Percolation_Upper = OUTVAR_nc_create("SW_Percolation_Upper", "mm", "upper soil layer water percolation", 0.1,
                                                           FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.SW_Percolation_Lower == 1)
    {
        *out_SW_Percolation_Lower = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_SW_Percolation_Lower, "SW_Percolation_Lower");
        OUTVAR_nc_initial(data_DEM, out_SW_Percolation_Lower, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_Percolation_Lower.nc");
        outnl_ncid->SW_Percolation_Lower = OUTVAR_nc_create("SW_Percolation_Lower", "mm", "lower soil layer water percolation", 0.1,
                                                           FP_OUT_VAR, time_steps_run, GP);
    }

    if (outnl.SW_SUB_Qin == 1)
    {
        *out_SW_SUB_Qin = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_SW_SUB_Qin, "SW_SUB_Qin");
        OUTVAR_nc_initial(data_DEM, out_SW_SUB_Qin, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_Qin.nc");
        outnl_ncid->SW_SUB_Qin = OUTVAR_nc_create("SW_SUB_Qin", "mm", "subsurface inflow from the grid cell", 0.1,
                                                 FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.SW_SUB_Qout == 1)
    {
        *out_SW_SUB_Qout = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_SW_SUB_Qout, "SW_SUB_Qout");
        OUTVAR_nc_initial(data_DEM, out_SW_SUB_Qout, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_Qout.nc");
        outnl_ncid->SW_SUB_Qout = OUTVAR_nc_create("SW_SUB_Qout", "mm", "subsurface outflow from the grid cell", 0.1,
                                                  FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.SW_SUB_z == 1)
    {
        *out_SW_SUB_z = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_SW_SUB_z, "SW_SUB_z");
        OUTVAR_nc_initial(data_DEM, out_SW_SUB_z, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_z.nc");
        outnl_ncid->SW_SUB_z = OUTVAR_nc_create("SW_SUB_z", "mm", "subsurface water table", 0.01,
                                               FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.SW_SUB_rise_lower == 1)
    {
        *out_SW_SUB_rise_lower = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_SW_SUB_rise_lower, "SW_SUB_rise_lower");
        OUTVAR_nc_initial(data_DEM, out_SW_SUB_rise_lower, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_rise_lower.nc");
        outnl_ncid->SW_SUB_rise_lower = OUTVAR_nc_create("SW_SUB_rise_lower", "mm", "water supplied by rising water table to lower soil layer", 0.1,
                                                        FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.SW_SUB_rise_upper == 1)
    {
        *out_SW_SUB_rise_upper = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_SW_SUB_rise_upper, "SW_SUB_rise_upper");
        OUTVAR_nc_initial(data_DEM, out_SW_SUB_rise_upper, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_rise_upper.nc");
        outnl_ncid->SW_SUB_rise_upper = OUTVAR_nc_create("SW_SUB_rise_upper", "mm", "water supplied by rising water table to upper soil layer", 0.1,
                                                        FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.SW_SUB_rf == 1)
    {
        *out_SW_SUB_rf = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_SW_SUB_rf, "SW_SUB_rf");
        OUTVAR_nc_initial(data_DEM, out_SW_SUB_rf, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_rf.nc");
        outnl_ncid->SW_SUB_rf = OUTVAR_nc_create("SW_SUB_rf", "mm", "water volume of returnflow", 0.1,
                                                FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.SW_SUB_Qc == 1)
    {
        *out_SW_SUB_Qc = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_SW_SUB_Qc, "SW_SUB_Qc");
        OUTVAR_nc_initial(data_DEM, out_SW_SUB_Qc, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "SW_SUB_Qc.nc");
        outnl_ncid->SW_SUB_Qc = OUTVAR_nc_create("SW_SUB_Qc", "mm", "lateral water into river channel", 0.1,
                                                FP_OUT_VAR, time_steps_run, GP);
    }
    if (outnl.Q_Channel == 1)
    {
        *out_Q_Channel = (int *)malloc(sizeof(int) * size);
        malloc_memory_error(*out_Q_Channel, "Q_Channel");
        OUTVAR_nc_initial(data_DEM, out_Q_Channel, HD);
        FP_OUT_VAR[0] = '\0';
        strcat(strcat(FP_OUT_VAR, GP.PATH_OUT), "Q_Channel.nc");
        outnl_ncid->Q_Channel = OUTVAR_nc_create("Q_Channel", "m3/s", "subsurface-induced discharge in river channels", 0.001,
                                                FP_OUT_VAR, time_steps_run, GP);
    }
}

void OUTVAR_nc_initial(
    int **data_DEM,
    int **out_data,
    ST_Header HD
)
{
    if (*(*data_DEM + 0) == HD.NODATA_value && *(*out_data + 0) != HD.NODATA_value)
    {
        /*****
         * **out_data: the raster array of the output variable,
         * initialize the raster array by specifying the NODATA value
         */
        for (size_t i = 0; i < HD.nrows; i++)
        {
            for (size_t j = 0; j < HD.ncols; j++)
            {
                if (*(*data_DEM + i * HD.ncols + j) == HD.NODATA_value)
                {
                    *(*out_data + i * HD.ncols + j) = HD.NODATA_value;
                }
            }
        }
    }
}

int OUTVAR_nc_create(
    char varNAME[],
    char att_unit[],
    char att_longname[],
    double scale_factor,
    char FP_output[],
    int ts_length,
    GLOBAL_PARA GP
    )
{
    int status_nc;
    int ncID_GEO;
    status_nc = nc_open(GP.FP_GEO, NC_NOWRITE, &ncID_GEO);
    if (status_nc != NC_NOERR)
    {
        printf("error in opening file%s: %s\n", GP.FP_GEO, nc_strerror(status_nc));
        exit(-1);
    }
    ST_Header HD;
    // int ncols,nrows;
    // double xllcorner, yllcorner, cellsize;
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "ncols", &HD.ncols);
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "nrows", &HD.nrows);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "xllcorner", &HD.xllcorner);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "yllcorner", &HD.yllcorner);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "cellsize", &HD.cellsize);
    int varID_lon, varID_lat, varID_DEM;
    double *data_lon;
    double *data_lat;
    int *data_DEM;
    data_lon = (double *)malloc(sizeof(double) * HD.ncols);
    data_lat = (double *)malloc(sizeof(double) * HD.nrows);
    data_DEM = (int *)malloc(sizeof(int) * HD.nrows * HD.ncols);

    nc_inq_varid(ncID_GEO, "DEM", &varID_DEM);
    nc_get_att_int(ncID_GEO, varID_DEM, "NODATA_value", &HD.NODATA_value);
    nc_get_var_int(ncID_GEO, varID_DEM, data_DEM);

    nc_inq_varid(ncID_GEO, "lon", &varID_lon);
    nc_get_var_double(ncID_GEO, varID_lon, data_lon);

    nc_inq_varid(ncID_GEO, "lat", &varID_lat);
    nc_get_var_double(ncID_GEO, varID_lat, data_lat);

    /******** output variable to be stored in NetCDF file *******/
    // define the NC file: variables, dimensions, attributes
    int ncID_out;
    status_nc = nc_create(FP_output, NC_CLOBBER, &ncID_out);
    if (status_nc != NC_NOERR)
    {
        printf("error in opening file%s: %s\n", FP_output, nc_strerror(status_nc));
        exit(-1);
    }
    int varID_out, varID_ts;
    int dimID_lon, dimID_lat, dimID_time;
    nc_def_dim(ncID_out, "time", NC_UNLIMITED, &dimID_time);
    nc_def_dim(ncID_out, "lon", HD.ncols, &dimID_lon);
    nc_def_dim(ncID_out, "lat", HD.nrows, &dimID_lat);

    int dims[3];
    dims[0] = dimID_time;
    dims[1] = dimID_lat;
    dims[2] = dimID_lon;
    nc_def_var(ncID_out, "lon", NC_DOUBLE, 1, &dimID_lon, &varID_lon);
    nc_def_var(ncID_out, "lat", NC_DOUBLE, 1, &dimID_lat, &varID_lat);
    nc_def_var(ncID_out, "time", NC_INT, 1, &dimID_time, &varID_ts); // long int (64-byte)

    nc_def_var(ncID_out, varNAME, NC_INT, 3, dims, &varID_out);
    nc_put_att_text(ncID_out, varID_out, "Units", 40L, att_unit);
    nc_put_att_text(ncID_out, varID_out, "long_name", 100L, att_longname);
    nc_put_att_double(ncID_out, varID_out, "scale_factor", NC_DOUBLE, 1, &scale_factor);
    nc_put_att_int(ncID_out, varID_out, "NODATA_value", NC_INT, 1, &HD.NODATA_value);
    nc_put_att_int(ncID_out, varID_out, "STEP_TIME(hours)", NC_INT, 1, &GP.STEP_TIME);
    nc_put_att_int(ncID_out, varID_out, "counts", NC_INT, 1, &ts_length);

    copy_global_attributes(ncID_GEO, ncID_out);
    nc_enddef(ncID_out);

    /********
     * write dimension variables: lat, lon, and time
     * */ 
    // the two dimensions: lon and lat
    nc_put_var_double(ncID_out, varID_lon, data_lon);
    nc_put_var_double(ncID_out, varID_lat, data_lat);
    // the time variable in NetCDF
    time_t start_time;
    struct tm tm_start;
    tm_start.tm_hour = GP.START_HOUR + 1;
    tm_start.tm_min = 0;
    tm_start.tm_sec = 0;
    tm_start.tm_mday = GP.START_DAY;
    tm_start.tm_mon = GP.START_MONTH - 1;
    tm_start.tm_year = GP.START_YEAR - 1900;
    // Daylight Saving Time not in effect;Specify UTC time zone
    tm_start.tm_isdst = 0;
    start_time = mktime(&tm_start);

    time_t *data_time;
    data_time = (time_t *)malloc(sizeof(time_t) * ts_length);
    for (size_t i = 0; i < ts_length; i++)
    {
        *(data_time + i) = start_time + 3600 * GP.STEP_TIME * i;
    }
    int start = 0;
    status_nc = nc_put_vara_long(ncID_out, varID_ts, &start, &ts_length, data_time);
    handle_error(status_nc, FP_output);

    nc_close(ncID_GEO);
    // nc_close(ncID_out);
    // printf("ncID_out: %d\n", ncID_out);
    return ncID_out;
}


void Write_Outnamelist(
    int t_run,
    OUT_NAME_LIST outnl,
    OUT_NAME_LIST outnl_ncid,
    ST_Header HD,
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
    int index_start[3] = {0, 0, 0};
    int index_count[3] = {1, 0, 0};
    index_count[1] = HD.nrows;
    index_count[2] = HD.ncols;
    index_start[0] = t_run;
    if (outnl.Rs == 1)
    {
        OUTVAR_nc_write(outnl_ncid.Rs, "Rs", out_Rs, index_start, index_count);
    }
    if (outnl.L_sky == 1)
    {
        OUTVAR_nc_write(outnl_ncid.L_sky, "L_sky", out_L_sky, index_start, index_count);
    }
    if (outnl.Rno == 1)
    {
        OUTVAR_nc_write(outnl_ncid.Rno, "Rno", out_Rno, index_start, index_count);
    }
    if (outnl.Rnu == 1)
    {
        OUTVAR_nc_write(outnl_ncid.Rnu, "Rnu", out_Rnu, index_start, index_count);
    }
    // ET variables
    if (outnl.Ep == 1)
    {
        OUTVAR_nc_write(outnl_ncid.Ep, "Ep", out_Ep, index_start, index_count);
    }
    if (outnl.EI_o == 1)
    {
        OUTVAR_nc_write(outnl_ncid.EI_o, "EI_o", out_EI_o, index_start, index_count);
    }
    if (outnl.EI_u == 1)
    {
        OUTVAR_nc_write(outnl_ncid.EI_u, "EI_u", out_EI_u, index_start, index_count);
    }
    if (outnl.ET_o == 1)
    {
        OUTVAR_nc_write(outnl_ncid.ET_o, "ET_o", out_ET_o, index_start, index_count);
    }
    if (outnl.ET_u == 1)
    {
        OUTVAR_nc_write(outnl_ncid.ET_u, "ET_u", out_ET_u, index_start, index_count);
    }
    if (outnl.ET_s == 1)
    {
        OUTVAR_nc_write(outnl_ncid.ET_s, "ET_s", out_ET_s, index_start, index_count);
    }
    if (outnl.Interception_o == 1)
    {
        OUTVAR_nc_write(outnl_ncid.Interception_o, "Interception_o", out_Interception_o, index_start, index_count);
    }
    if (outnl.Interception_u == 1)
    {
        OUTVAR_nc_write(outnl_ncid.Interception_u, "Interception_u", out_Interception_u, index_start, index_count);
    }
    if (outnl.Prec_net == 1)
    {
        OUTVAR_nc_write(outnl_ncid.Prec_net, "Prec_net", out_Prec_net, index_start, index_count);
    }
    // soil variables
    if (outnl.SM_Lower == 1)
    {
        OUTVAR_nc_write(outnl_ncid.SM_Lower, "SM_Lower", out_SM_Lower, index_start, index_count);
    }
    if (outnl.SM_Upper == 1)
    {
        OUTVAR_nc_write(outnl_ncid.SM_Upper, "SM_Upper", out_SM_Upper, index_start, index_count);
    }
    if (outnl.SW_Infiltration == 1)
    {
        OUTVAR_nc_write(outnl_ncid.SW_Infiltration, "SW_Infiltration", out_SW_Infiltration, index_start, index_count);
    }
    if (outnl.SW_Percolation_Upper == 1)
    {
        OUTVAR_nc_write(outnl_ncid.SW_Percolation_Upper, "SW_Percolation_Upper", out_SW_Percolation_Upper, index_start, index_count);
    }
    if (outnl.SW_Percolation_Lower == 1)
    {
        OUTVAR_nc_write(outnl_ncid.SW_Percolation_Lower, "SW_Percolation_Lower", out_SW_Percolation_Lower, index_start, index_count);
    }

    if (outnl.SW_SUB_Qin == 1)
    {
        OUTVAR_nc_write(outnl_ncid.SW_SUB_Qin, "SW_SUB_Qin", out_SW_SUB_Qin, index_start, index_count);
    }
    if (outnl.SW_SUB_Qout == 1)
    {
        OUTVAR_nc_write(outnl_ncid.SW_SUB_Qout, "SW_SUB_Qout", out_SW_SUB_Qout, index_start, index_count);
    }
    if (outnl.SW_SUB_z == 1)
    {
        OUTVAR_nc_write(outnl_ncid.SW_SUB_z, "SW_SUB_z", out_SW_SUB_z, index_start, index_count);
    }
    if (outnl.SW_SUB_rise_lower == 1)
    {
        OUTVAR_nc_write(outnl_ncid.SW_SUB_rise_lower, "SW_SUB_rise_lower", out_SW_SUB_rise_lower, index_start, index_count);
    }
    if (outnl.SW_SUB_rise_upper == 1)
    {
        OUTVAR_nc_write(outnl_ncid.SW_SUB_rise_upper, "SW_SUB_rise_upper", out_SW_SUB_rise_upper, index_start, index_count);
    }
    if (outnl.SW_SUB_rf == 1)
    {
        OUTVAR_nc_write(outnl_ncid.SW_SUB_rf, "SW_SUB_rf", out_SW_SUB_rf, index_start, index_count);
    }
    if (outnl.SW_SUB_Qc == 1)
    {
        OUTVAR_nc_write(outnl_ncid.SW_SUB_Qc, "SW_SUB_Qc", out_SW_SUB_Qc, index_start, index_count);
    }
    if (outnl.Q_Channel == 1)
    {
        OUTVAR_nc_write(outnl_ncid.Q_Channel, "Q_Channel", out_Q_Channel, index_start, index_count);
    }
}

void OUTVAR_nc_write(
    int ncID,
    char varNAME[],
    int **out_data,
    int index_start[3],
    int index_count[3]
)
{
    int varID;
    nc_inq_varid(ncID, varNAME, &varID);
    nc_put_vara_int(ncID, varID, index_start, index_count, *out_data);
}

void OUTVAR_nc_close(
    OUT_NAME_LIST outnl,
    OUT_NAME_LIST outnl_ncid)
{
    // radiation variables
    if (outnl.Rs == 1)
    {
        nc_close(outnl_ncid.Rs);
    }
    if (outnl.L_sky == 1)
    {
        nc_close(outnl_ncid.L_sky);
    }
    if (outnl.Rno == 1)
    {
        nc_close(outnl_ncid.Rno);
    }
    if (outnl.Rnu == 1)
    {
        nc_close(outnl_ncid.Rnu);
    }
    // ET variables
    if (outnl.Ep == 1)
    {
        nc_close(outnl_ncid.Ep);
    }
    if (outnl.EI_o == 1)
    {
        nc_close(outnl_ncid.EI_o);
    }
    if (outnl.EI_u == 1)
    {
        nc_close(outnl_ncid.EI_u);
    }
    if (outnl.ET_o == 1)
    {
        nc_close(outnl_ncid.ET_o);
    }
    if (outnl.ET_u == 1)
    {
        nc_close(outnl_ncid.ET_u);
    }
    if (outnl.ET_s == 1)
    {
        nc_close(outnl_ncid.ET_s);
    }
    if (outnl.Interception_o == 1)
    {
        nc_close(outnl_ncid.Interception_o);
    }
    if (outnl.Interception_u == 1)
    {
        nc_close(outnl_ncid.Interception_u);
    }
    if (outnl.Prec_net == 1)
    {
        nc_close(outnl_ncid.Prec_net);
    }
    // soil variables
    if (outnl.SM_Lower == 1)
    {
        nc_close(outnl_ncid.SM_Lower);
    }
    if (outnl.SM_Upper == 1)
    {
        nc_close(outnl_ncid.SM_Upper);
    }
    if (outnl.SW_Infiltration == 1)
    {
        nc_close(outnl_ncid.SW_Infiltration);
    }
    if (outnl.SW_Percolation_Upper == 1)
    {
        nc_close(outnl_ncid.SW_Percolation_Upper);
    }
    if (outnl.SW_Percolation_Lower == 1)
    {
        nc_close(outnl_ncid.SW_Percolation_Lower);
    }
    if (outnl.SW_SUB_Qin == 1)
    {
        nc_close(outnl_ncid.SW_SUB_Qin);
    }
    if (outnl.SW_SUB_Qout == 1)
    {
        nc_close(outnl_ncid.SW_SUB_Qout);
    }
    if (outnl.SW_SUB_z == 1)
    {
        nc_close(outnl_ncid.SW_SUB_z);
    }
    if (outnl.SW_SUB_rise_lower == 1)
    {
        nc_close(outnl_ncid.SW_SUB_rise_lower);
    }
    if (outnl.SW_SUB_rise_upper == 1)
    {
        nc_close(outnl_ncid.SW_SUB_rise_upper);
    }
    if (outnl.SW_SUB_rf == 1)
    {
        nc_close(outnl_ncid.SW_SUB_rf);
    }
    if (outnl.SW_SUB_Qc == 1)
    {
        nc_close(outnl_ncid.SW_SUB_Qc);
    }
    if (outnl.Q_Channel == 1)
    {
        nc_close(outnl_ncid.Q_Channel);
    }
}

void Write2NC_Outnamelist(
    OUT_NAME_LIST outnl,
    int time_steps_run,
    int **out_SW_Run_Infil,
    int **out_SW_Run_Satur,
    GLOBAL_PARA GP
)
{
    char FP_OUT_VAR[MAXCHAR];
    
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
