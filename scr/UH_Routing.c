/*
 * SUMMARY:      UH_Routing.c
 * USAGE:        use UH to route the surface runoff
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Jan-2024
 * DESCRIPTION:  simulate the surface runoff-induced discharge by using unit hydrograph
 * DESCRIP-END.
 * FUNCTIONS:    UH_Read(), UH_Import(), UH_Routing()
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
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "GEO_ST.h"
#include "NC_copy_global_att.h"
#include "UH_Generation.h"
#include "UH_Routing.h"

void UH_Read(
    int ncID_UH,
    int *varID_UH,
    int *outlet_count,
    int *outlet_index_row,
    int *outlet_index_col,
    int *UH_steps
)
{
    int status_nc;
    char varUH_Name[20];
    char var_num[10];
    status_nc = nc_get_att_int(ncID_UH, NC_GLOBAL, "outlet_count", outlet_count);
    handle_error(status_nc, "UH.nc");
    for (size_t i = 0; i < *outlet_count; i++)
    {
        varUH_Name[0] = '\0';
        sprintf(var_num, "%d", i); // convert an integer into string
        strcat(strcat(varUH_Name, "UH"), var_num);  // concatenate the strings
        status_nc = nc_inq_varid(ncID_UH, varUH_Name, (varID_UH + i)); handle_error(status_nc, "UH.nc");
        nc_get_att_int(ncID_UH, *(varID_UH + i), "UH_steps", (UH_steps + i)); handle_error(status_nc, "UH.nc");
        nc_get_att_int(ncID_UH, *(varID_UH + i), "outlet_index_row", (outlet_index_row + i)); handle_error(status_nc, "UH.nc");
        nc_get_att_int(ncID_UH, *(varID_UH + i), "outlet_index_col", (outlet_index_col + i)); handle_error(status_nc, "UH.nc");
    }
}

void UH_Import(
    int ncID_UH,
    int *varID_UH,
    int outlet_count,
    int cell_counts_total,
    int *UH_steps,
    double **data_UH
)
{
    int index_begin = 0;
    for (size_t i = 0; i < outlet_count; i++)
    {
        printf("varID_UH: %d\nUH_STEPs: %d\n", *(varID_UH + i), *(UH_steps + i));
        nc_get_var_double(
            ncID_UH, *(varID_UH + i), (*data_UH + index_begin)
        );
        index_begin += *(UH_steps + i) * cell_counts_total;
    }
}


void UH_Routing(
    int *data_RUNOFF_sf,  // unit: 0.1mm 
    double *data_UH,
    double *Qout,
    int UH_steps,
    int ncols,
    int nrows,
    int time_steps_run,
    int cellsize_m,
    int NODATA_value,
    int STEP_TIME
)
{
    double cell_area;
    int index_geo;
    int index_uh;
    int index_run;

    int cell_counts_total;
    cell_counts_total = ncols * nrows;
    cell_area = cellsize_m * cellsize_m;

    for (int r = 0; r < time_steps_run; r++)
    {
        // each simulation step
        *(Qout + r) = 0.0;
        for (int t = 0; t < UH_steps; t++)
        {
            // each UH step
            if (r - t >= 0)
            {
                // check:
                // the beginning steps within the length of UH series
                index_uh = t * cell_counts_total;
                index_run = (r - t) * cell_counts_total;
                for (size_t i = 0; i < nrows; i++)
                {
                    for (size_t j = 0; j < ncols; j++)
                    {
                        index_geo = i * ncols + j;
                        if (IsNODATA(*(data_UH + index_geo), NODATA_value) != 1)
                        {
                            // data_RUNOFF_sf: unit: 0.1 mm -> m
                            *(Qout + r) += *(data_UH + index_uh + index_geo) * *(data_RUNOFF_sf + index_run + index_geo) / 10000; 
                        }
                    }
                }
            }
        }
        *(Qout + r) = *(Qout + r) * cell_area * STEP_TIME; // unit: m3/h
    }
}

int IsNODATA(
    double value,
    int NODATA_value
)
{
    /*****
     * check whether the double number equals 
     * or is close enough to the NODATA_value:
     * 1: close enough
     * 0: not
     * */ 
    double y;
    int result;
    y = value - NODATA_value;
    if (y < 0)
    {
        y = -y;
    }

    if (y < 0.00001)
    {
        result = 1;
    }
    else
    {
        result = 0;
    }
    return result;
}



// int main(int argc, char const *argv[])
// {
//     int ncID_UH;
//     int ncols, nrows;
//     nc_open("D:/xHM/example_data/CT_GEO_250m/UH.nc", NC_NOWRITE, &ncID_UH);
//     nc_get_att_int(ncID_UH, NC_GLOBAL, "ncols", &ncols);
//     nc_get_att_int(ncID_UH, NC_GLOBAL, "nrows", &nrows);
//     int varID_UH[MAX_OUTLETS];
//     int outlet_count;
//     int outlet_index_row[MAX_OUTLETS];
//     int outlet_index_col[MAX_OUTLETS];
//     int UH_steps[MAX_OUTLETS];
//     int UH_steps_total = 0;
//     UH_Read(
//         ncID_UH,
//         varID_UH,
//         &outlet_count,
//         outlet_index_row,
//         outlet_index_col,
//         UH_steps);
//     printf("outlet_count: %d\n", outlet_count);
//     printf("%6s%6s%6s%6s\n", "outlet", "row", "col", "steps");
//     for (size_t i = 0; i < outlet_count; i++)
//     {
//         printf("%6d%6d%6d%6d\n", i, outlet_index_row[i], outlet_index_col[i], UH_steps[i]);
//         UH_steps_total += UH_steps[i];
//     }
//     printf("UH_steps_total: %d\n", UH_steps_total);
//     int cell_counts_total;
//     cell_counts_total = ncols * nrows;
//     printf("cell_counts_total: %d\n", cell_counts_total);
//     double *data_UH;
//     data_UH = (double *)malloc(sizeof(double) * cell_counts_total * UH_steps_total);

//     UH_Import(
//         ncID_UH,
//         varID_UH,
//         outlet_count,
//         cell_counts_total,
//         UH_steps,
//         &data_UH);
//     // int t = 97-1 + 59;
//     // for (size_t i = 367; i < 377; i++)
//     // {
//     //     for (size_t j = 37; j < 42; j++)
//     //     {
//     //         printf("%8.4f ", *(data_UH + t * cell_counts_total + i * ncols + j));
//     //     }
//     //     printf("\n");
//     // }
    
//     return 0;
// }

