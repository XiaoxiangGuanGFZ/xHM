/*
 * SUMMARY:      Route_Outlet.c
 * USAGE:        calculate the total streamflow at the outlets
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Jan-2024
 * DESCRIPTION:  simulate the total streamflow at outlets, including 
 *               overland surface runoff and subsurface saturated soil flow 
 * DESCRIP-END.
 * FUNCTIONS:    Route_Outlet(), Write_Qout()
 *
 * COMMENTS:
 * - UH_Read():          read the main UH attributes from UH.nc
 * - UH_Import():        import the UH fromUH.nc file
 * - UH_Routing():       route the surface runoff using UH
 * - IsNODATA():         
 *
 * REFERENCES:
 *
 *
 */

/*******************************************************************************
 * VARIABLEs:
 * double *Qout_SF_Infil            - overland surface flow from infiltration-excess, [m3/h - m3/s]
 * double *Qout_SF_Satur            - overland surface flow from saturation-excess, [m3/h - m3/s]
 * double *Qout_Sub                 - subsurface flow from ground water, [m3/h - m3/s]
 * double *Qout_outlet              - streamflow at defined outlets, [m3/s]
 * int outlet_count                 - number of outlets
 * int time_steps_run               - the length (steps) of simulation
 * int *outlet_index_row            - point to the row numbers of the outlets, [location]
 * int *outlet_index_col            - point to the column numbers of the oulets, [location]
 * char PATH_OUT[]                  - the work path where the output files are going to be saved
 * OUT_NAME_LIST outnl              - output variable name list structure, see "HM_ST.h"
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "HM_ST.h"
#include "Constants.h"
#include "Route_Outlet.h"

void Route_Outlet(
    double *Qout_SF_Infil,
    double *Qout_SF_Satur,
    double *Qout_Sub,
    double *Qout_outlet,
    int outlet_count,
    int time_steps_run
)
{
    /*********************************
     * sum up the total streamflow at outlets:
     * - overland surface flow from infiltration-excess
     * - overland surface flow from saturation-excess
     * - subsurface water flow in saturated soil zone
     * 
    */
    for (size_t i = 0; i < time_steps_run * outlet_count; i++)
    {
        // convert the unit from m3/h to m3/s
        *(Qout_SF_Infil + i) /= 3600;
        *(Qout_SF_Satur + i) /= 3600;
        *(Qout_Sub + i) /= 3600;
        *(Qout_outlet + i) = *(Qout_SF_Infil + i) +
                             *(Qout_SF_Satur + i) +
                             *(Qout_Sub + i);
    }
}


void Write_Qout(
    OUT_NAME_LIST outnl,
    char PATH_OUT[],
    double *Qout_SF_Infil,
    double *Qout_SF_Satur,
    double *Qout_Sub,
    double *Qout_outlet,
    int *outlet_index_row,
    int *outlet_index_col,
    int outlet_count,
    int time_steps_run
)
{
    /***************************************
     * write the streamflow at outlets into text files
    */
    int num_Qout;
    num_Qout = outnl.Qout_outlet + outnl.Qout_SF_Infil + outnl.Qout_SF_Satur + outnl.Qout_Sub;
    if (num_Qout >= 1)
    {
        
    }
    
    FILE *fp;
    char FP[MAXCHAR];
    int index_Qout = 0;
    char numberString[4];
    for (size_t s = 0; s < outlet_count; s++)
    {
        FP[0] = '\0';
        sprintf(numberString, "%d", s);
        strcat(strcat(strcat(FP, PATH_OUT), "outlet"), numberString);
        if ((fp = fopen(FP, "w")) == NULL)
        {
            printf("File Error: cannot create or open output file: %s\n", FP);
            exit(0);
        }
        /***** write the header *****/
        fprintf(fp, "# outlet: %d\n# row: %d\n# col: %d\n# unit: m3/s\n# length: %d\n# variables: \n",
                s, outlet_index_row[s], outlet_index_col[s], time_steps_run);
        if (outnl.Qout_SF_Infil == 1)
        {
            fprintf(fp, "# - Qout_SF_Infil\n");
        }
        if (outnl.Qout_Sub == 1)
        {
            fprintf(fp, "# - Qout_Sub\n");
        }
        if (outnl.Qout_SF_Satur == 1)
        {
            fprintf(fp, "# - Qout_SF_Satur\n");
        }
        if (outnl.Qout_outlet == 1)
        {
            fprintf(fp, "# - Qout_outlet\n");
        }
        /***** write the data *****/
        for (size_t i = 0; i < time_steps_run; i++)
        {
            if (outnl.Qout_SF_Infil == 1)
            {
                fprintf(fp, "%.3f ", *(Qout_SF_Infil + index_Qout + i));
            }
            if (outnl.Qout_Sub == 1)
            {
                fprintf(fp, "%.3f ", *(Qout_Sub + index_Qout + i));
            }
            if (outnl.Qout_SF_Satur == 1)
            {
                fprintf(fp, "%.3f ", *(Qout_SF_Satur + index_Qout + i));
            }
            if (outnl.Qout_outlet == 1)
            {
                fprintf(fp, "%.3f ", *(Qout_outlet + index_Qout + i));
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
        index_Qout += time_steps_run;
    }
}