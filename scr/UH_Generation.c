/*
 * SUMMARY:      UH_Generation.c
 * USAGE:        Calculate overland runoff routing using Unit Hydrograph (UH) 
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  Calculate overland runoff using UH, over a gridded surface. 
 *               the UH is derived based on topography (DEM). 
 * DESCRIP-END.
 * FUNCTIONS:    Grid_Slope(), Grid_SlopeArea(), Grid_Velocity()
 *                  Grid_FlowTime(), Grid_UH(), Grid_Outlets(), 
 *                  Grid_OutletMask(), UH_Generation()
 * 
 * COMMENTS:
 * - Grid_Slope():          derive the slope from DEM and flow direction (D8)
 * - Grid_SlopeArea():      compute the slope-area term for each grid cell
 * - Grid_Velocity():       assign flow velocity to each grid cell
 * - Grid_FlowTime():       compute the flow time of each grid to the outlet
 * - Grid_UH():             generate UH for each grid cell
 * - Grid_Outlets():        extract the number and coordinates (row and col index) of outlets
 * - Grid_OutletMask():     extract the mask (the upstream region) of an outlet (based on coordinates)
 * - UH_Generation():       generate the Unit Hydrograph for multiple outlets
 * REFERENCES:
 * 
 *
 */

/*****************************************************************************
 * VARIABLEs:
 * int *data_DEM             - point to the gridded 2D DEM data
 * int *data_FDR             - point to the gridded 2D dlow direction data
 * int *data_FAC             - point to the gridded 2D flow accumulation data
 * double *data_Slope        - point to the gridded 2D slope data
 * double *data_FlowDistance - point to the gridded 2D flow distance (between two neighboring grid cells) data, [m] 
 * double *data_SlopeArea    - point to the gridded 2D flow-area term data
 * double *slope_area_avg    - point to the basin-average slope-area term
 * double *data_V            - point to the gridded 2D flow velocity data [m/h]
 * double V_avg              - parameter, indicating the basin-average flow velocity [m/h]
 * double V_max              - parameter, indicating the basin-maximum flow velocity [m/h]
 * double V_min              - parameter, indicating the basin-minimum flow velocity [m/h]
 * double b, c               - parameters in slope-area term formula
 * int *data_Outlet          - point to the gridded 2D outlet data, 1: this is an outlet
 * int outlet_index_row[]    - 1D array: row index for the outlets, the index starting from 0
 * int outlet_index_col[]    - 1D array: col index for the outlets, the index starting from 0
 * int *data_Mask            - point to the gridded 2D mask data for one specific outlet
 * int outlet_count          - number of outlets
 * int ncols                 - nunber of the columns of the gridded 2D data
 * int nrows                 - nunber of the rows of the gridded 2D data
 * int NODATA_value          - the missing data value
 * int cellsize_m            - the size of the grid cell, in meters
 * int time_steps            - the steps(length) of a UH 
 * double beta               - parameter in h(t) formula in UH, 
 *                             ratio of the residence time flow water in the reservoir to the total flow time
 * double **data_UH          - a pointer to a pointer pointing to the gridded 2D UH data for a specific outlet
 * 
******************************************************************************/


#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <netcdf.h>
#include <string.h>
#include "GEO_ST.h"
#include "UH_Generation.h"
#include "NC_copy_global_att.h"

void Grid_Slope(
    int *data_DEM,
    int *data_FDR,
    double *data_Slope,
    double *data_FlowDistance,
    int ncols,
    int nrows,
    int NODATA_value,
    int cellsize_m
)
{
    int i,j;
    int z_1, z_2;
    double flow_distance;
    int dir;
    double slope;
    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            z_1 = *(data_DEM + i * ncols + j);
            if (z_1 == NODATA_value)
            {
                slope = (double) NODATA_value;
                flow_distance = (double) NODATA_value;
            }
            else
            {
                dir = *(data_FDR + i * ncols + j);
                switch (dir)
                {
                case 1:
                    {
                        z_2 = *(data_DEM + i * ncols + j + 1);
                        flow_distance = (double) cellsize_m;
                        break;
                    }
                    
                case 2:
                    {
                        z_2 = *(data_DEM + (i + 1) * ncols + j + 1);
                        flow_distance = (double) cellsize_m * 1.41;
                        break;
                    }
                    
                case 4:
                    {
                        z_2 = *(data_DEM + (i + 1) * ncols + j);
                        flow_distance = (double) cellsize_m;
                        break;
                    }
                case 8:
                    {
                        z_2 = *(data_DEM + (i + 1) * ncols + j - 1);
                        flow_distance = (double)cellsize_m * 1.41;
                        break;
                    }
                case 16:
                    {
                        z_2 = *(data_DEM + i * ncols + j - 1);
                        flow_distance = (double) cellsize_m;
                        break;
                    }
                case 32:
                    {
                        z_2 = *(data_DEM + (i - 1) * ncols + j - 1);
                        flow_distance = (double) cellsize_m * 1.41;
                        break;
                    }
                case 64:
                    {
                        z_2 = *(data_DEM + (i - 1) * ncols + j);
                        flow_distance = (double) cellsize_m;
                        break;
                    }
                case 128:
                    {
                        z_2 = *(data_DEM + (i - 1) * ncols + j + 1);
                        flow_distance = (double) cellsize_m * 1.41;
                        break;
                    }
                default:
                    {
                        printf("Unrecognized flow direction field value at the location row: %d, col: %d!", i+1, j+1);
                        exit(0);
                    }
                }
                if (z_2 == NODATA_value)
                {
                    // the outlet 
                    z_2 = z_1;
                }
                slope = fabs(z_2 - z_1) / flow_distance;
            }
            *(data_Slope + i * ncols + j) = slope;
            *(data_FlowDistance + i * ncols + j) = flow_distance;
        }        
    }  
    printf("Grid_Slope: done!\n");  
}


void Grid_SlopeArea(
    int *data_FAC,
    double *data_Slope,
    double *data_SlopeArea,
    double *slope_area_avg,
    double b,
    double c,
    int ncols,
    int nrows,
    int NODATA_value,
    int cellsize_m
)
{
    double slope_area_sum = 0.0;
    double slope_area;
    double cell_area;
    cell_area = cellsize_m * cellsize_m;
    
    double area_acc;
    double slope;
    int i,j;
    int cell_count = 0;

    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            if (*(data_FAC + i * ncols + j) == NODATA_value)
            {
                slope_area = (double) NODATA_value;
            }
            else
            {
                cell_count++;
                slope = *(data_Slope + i * ncols + j);
                area_acc = *(data_FAC + i * ncols + j) * cell_area;
                slope_area = pow(slope, b) * pow(area_acc, c);
                slope_area_sum += slope_area;
            }
            *(data_SlopeArea + i * ncols + j) = slope_area;
        }
    }
    *slope_area_avg = slope_area_sum / cell_count;
    printf("Grid_SlopeArea: done!\n");
}

void Grid_Velocity(
    int *data_FAC,
    double *data_SlopeArea,
    double slope_area_avg,
    double *data_V,
    double V_avg,
    double V_max,
    double V_min,
    int ncols,
    int nrows,
    int NODATA_value
)
{
    int i,j;
    double V;
    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            if (*(data_FAC + i * ncols + j) == NODATA_value)
            {
                V = (double) NODATA_value;
            }
            else
            {
                V = *(data_SlopeArea + i * ncols + j) / slope_area_avg * V_avg;
                
                if (V > V_max)
                {
                    V = V_max;
                }
                if (V < V_min)
                {
                    V = V_min;
                }
            }
            *(data_V + i * ncols + j) = V;
        }
    }
    printf("Grid_Velocity: done!\n");
}

void Grid_Outlets(
    int *data_Outlet,
    int outlet_index_row[],
    int outlet_index_col[],
    int *outlet_count,
    int ncols,
    int nrows,
    int NODATA_value
)
{
    /***********************************
     * identify the outlets (from data_Outlet)
     * 1: it is a outlet,
     * 0: it is not;
     * NODATA_value: no data
     *
     * count the number of outlets and retrieve the row and col index
     * of the outlets.
     *
     */
    int i, j;
    int outlet_value;
    *outlet_count = 0;

    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            if (*outlet_count <= MAX_OUTLETS)
            {
                outlet_value = *(data_Outlet + i * ncols + j);
                if (outlet_value == 1)
                {
                    outlet_index_row[*outlet_count] = i;
                    outlet_index_col[*outlet_count] = j;
                    (*outlet_count) += 1;
                }
            } else {
                printf("The number of outlets exceeds the defined maximum! Increase the contstant MAX_OUTLETS in Route_UH.h\n");
                exit(0);
            }
        }
    }
    printf("Grid_Outlets: done!\n");
}

void Grid_OutletMask(
    int outlet_index_row,
    int outlet_index_col,
    int *data_FDR,
    int *data_Mask,
    int ncols,
    int nrows,
    int NODATA_value
)
{
    /****************************
     * extract the upstream region (mask) of the outlet
     * based on the coordinate (row and col index) of outlet,
     * a gridded mask data is generated.
    */
    int i, j;
    
    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            // preset all cell as NODATA_value
            *(data_Mask + i * ncols + j) = NODATA_value;
            
        }
    }
    // this is the outlet
    *(data_Mask + outlet_index_row * ncols + outlet_index_col) = 1;

    int ite_i, ite_j;
    int cell_in;
    int fdr;
    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            ite_i = i; ite_j = j;
            fdr = *(data_FDR + ite_i * ncols + ite_j);
            if (fdr != NODATA_value)
            {
                cell_in = 0;
                while (ite_i < nrows && ite_i >=0 && ite_j < ncols && ite_j >= 0)
                {
                    // iterate cell within the domain
                    if (
                        *(data_Mask + ite_i * ncols + ite_j) == 1
                    )
                    {
                        /**********
                         * if a grid cell points (directs) to a cell within 
                         *      the upstream region of the outlet, then, the grid cell itself is 
                         *      also located within this mask.
                         * */ 
                        cell_in = 1;
                        break; // jump out from the while loop
                    }
                    else if (*(data_Mask + ite_i * ncols + ite_j) == 0)
                    {
                        cell_in = 0;
                        break;
                    }                    
                    else
                    {
                        /**********
                         * search the next one along the flow direction
                        */
                        switch (fdr)
                        {
                        case 1:
                        {
                            ite_j += 1;
                            break;
                        }
                        case 2:
                        {
                            ite_i += 1;
                            ite_j += 1;
                            break;
                        }
                        case 4:
                        {
                            ite_i += 1;
                            break;
                        }
                        case 8:
                        {
                            ite_i += 1;
                            ite_j -= 1;
                            break;
                        }
                        case 16:
                        {
                            ite_j -= 1;
                            break;
                        }
                        case 32:
                        {
                            ite_i -= 1;
                            ite_j -= 1;
                            break;
                        }
                        case 64:
                        {
                            ite_i -= 1;
                            break;
                        }
                        case 128:
                        {
                            ite_i -= 1;
                            ite_j += 1;
                            break;
                        }
                        default:
                        {
                            printf("Unrecognized flow direction field value at the location row: %d, col: %d!\n", i + 1, j + 1);
                            exit(0);
                        }
                        }
                        fdr = *(data_FDR + ite_i * ncols + ite_j);
                        if (fdr == NODATA_value)
                        {
                            cell_in = 0;
                            break;  // jump out from the while loop
                        }
                    }
                }
                if (cell_in == 1)
                {
                    *(data_Mask + i * ncols + j) = 1;
                }
                else
                {
                    *(data_Mask + i * ncols + j) = 0;
                }
            }        
        }        
    }

    /*******************
     * turn all 0 into NODATA_value,
     * exclude the grid cells not located in the outlet-mask
    */
    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            if (*(data_Mask + i * ncols + j) == 0)
            {
                *(data_Mask + i * ncols + j) = NODATA_value;
            }
        }
    }
    printf("Grid_OutletMask: done!\n");
}


void Grid_FlowTime(
    int *data_mask,
    int *data_FDR,
    double *data_V,
    double *data_FlowDistance,
    double *data_FlowTime,
    int outlet_index_row,
    int outlet_index_col,
    int ncols,
    int nrows,
    int NODATA_value
)
{
    int i,j;
    int dir;
    int trace_i, trace_j;
    double FlowTime;
    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            if (*(data_mask + i * ncols + j) == NODATA_value)
            {
                FlowTime = (double) NODATA_value;
            }
            else
            {
                trace_i = i; trace_j = j;
                FlowTime = *(data_FlowDistance + trace_i * ncols + trace_j) / *(data_V + trace_i * ncols + trace_j);
                while (!(trace_i == outlet_index_row && trace_j == outlet_index_col))
                {
                    dir = *(data_FDR + trace_i * ncols + trace_j);
                    switch (dir)
                    {
                    case 1:
                    {
                        trace_j += 1;
                        break;
                    }
                    case 2:
                    {
                        trace_i += 1;
                        trace_j += 1;
                        break;
                    }
                    case 4:
                    {
                        trace_i += 1;
                        break;
                    }
                    case 8:
                    {
                        trace_i += 1;
                        trace_j -= 1;
                        break;
                    }
                    case 16:
                    {
                        trace_j -= 1;
                        break;
                    }
                    case 32:
                    {
                        trace_i -= 1;
                        trace_j -= 1;
                        break;
                    }
                    case 64:
                    {
                        trace_i -= 1;
                        break;
                    }
                    case 128:
                    {
                        trace_i -= 1;
                        trace_j += 1;
                        break;
                    }
                    default:
                        printf("Unrecognized flow direction field value at the location row: %d, col: %d!", i + 1, j + 1);
                        exit(0);
                    }
                    FlowTime += *(data_FlowDistance + trace_i * ncols + trace_j) / *(data_V + trace_i * ncols + trace_j);

                }
                
            }
            *(data_FlowTime + i * ncols + j) = FlowTime;   
        }
    }
    printf("Grid_FlowTime: done!\n");
}

void Grid_UH(
    int *data_mask,
    double *data_FlowTime,
    double **data_UH,
    int *time_steps,
    double beta,
    int step_time,
    int ncols,
    int nrows,
    int NODATA_value
)
{
    int i,j,t;
    double FlowTime_max = 0.0;
    double FlowTime_cell;
    double step;
    step = (double)step_time;
    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            if (*(data_mask + i * ncols + j) != NODATA_value)
            {
                FlowTime_cell = *(data_FlowTime + i * ncols + j);
                if (FlowTime_max < FlowTime_cell)
                {
                    FlowTime_max = FlowTime_cell;
                    // printf("FlowTime_max: %.3f\n", FlowTime_max);
                }
            }
        }
    }
    *time_steps = (int) (FlowTime_max / step_time) * 4 + 1;
    
    // printf("time_steps: %d\n", *time_steps);
    // printf("size of double: %d\n", sizeof(double));
    // printf("size: %d\n", (*time_steps) * ncols * nrows);
    *data_UH = (double *)malloc(sizeof(double) * (*time_steps) * ncols * nrows);
    double UH_value;
    double Ts, Tr;
    for (t = 0; t < *time_steps; t++)
    {
        for (i = 0; i < nrows; i++)
        {
            for (j = 0; j < ncols; j++)
            {
                if (*(data_mask + i * ncols + j) == NODATA_value)
                {
                    UH_value = (double)NODATA_value;
                }
                else 
                {
                    FlowTime_cell = *(data_FlowTime + i * ncols + j);
                    Ts = FlowTime_cell * (1 - beta);
                    Tr = FlowTime_cell * beta;
                    if ( (t + 1) * step < Ts)
                    {
                        UH_value = 0.0;
                    } else if ((t + 1) * step <= (Ts + step))
                    {
                        UH_value = (1 / step) * (exp(1) - exp(1 - ((t + 1) * step - Ts) / Tr));
                    } else {
                        UH_value = (1 / step) * exp(- ((t + 1) * step - Ts) / Tr) * (exp(step / Tr) - 1);
                    }
                }
                
                *(*data_UH + t * ncols * nrows + i * ncols + j) = UH_value;
            }
        }
    }
    UH_scale(data_mask, data_UH, *time_steps, step_time, ncols, nrows, NODATA_value);
    printf("Grid_UH: done!\n");
}

void UH_scale(
    int *data_mask,
    double **data_UH,
    int time_steps,
    int step_time,
    int ncols,
    int nrows,
    int NODATA_value
)
{
    int index;
    int n;
    n = ncols * nrows;
    double *sum;
    sum = (double *)malloc(sizeof(double) * n);

    /*********
     * calculate the sum (toals) of UH series
     * for each grid cell.
     */
    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
        {
            index = i * ncols + j;
            if (*(data_mask + index) == NODATA_value)
            {
                *(sum + index) = (double)NODATA_value;
            }
            else
            {
                *(sum + index) = 0.0;
                for (size_t t = 0; t < time_steps; t++)
                {
                    *(sum + index) += *(*data_UH + t * n + index);
                }
                // printf("%.6f\n", *(sum + index));
            }
        }
    }
    /**************
     * scale
     */
    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
        {
            index = i * ncols + j;
            if (*(data_mask + index) != NODATA_value)
            {
                for (size_t t = 0; t < time_steps; t++)
                {
                    *(*data_UH + t * n + index) = *(*data_UH + t * n + index) * 1.0 / step_time / *(sum + index);
                }
            }
        }
    }
    free(sum);
}

void UH_Generation(
    char FP_GEO[],
    char FP_UH[],
    int step_time,
    double Velocity_avg,
    double Velocity_max, 
    double Velocity_min,
    double b, 
    double c
)
{
    /*****
     * new_UH == 1: regenerate UH anyway,
     * new_UH == 0: no operation
     */
    int new_UH = 0;
    int ncID_UH;
    int status_nc;
    status_nc = nc_open(FP_UH, NC_WRITE, &ncID_UH);
    if (status_nc == 2)
    {
        // No such file or directory
        new_UH = 1;
    }
    else
    {
        // check other errors
        handle_error(status_nc, FP_UH);
        int step_time_old;
        double Velocity_avg_old, Velocity_max_old, Velocity_min_old;
        double b_old, c_old;
        nc_get_att_double(ncID_UH, NC_GLOBAL, "b", &b_old);
        nc_get_att_double(ncID_UH, NC_GLOBAL, "c", &c_old);
        nc_get_att_int(ncID_UH, NC_GLOBAL, "STEP_TIME", &step_time_old);
        nc_get_att_double(ncID_UH, NC_GLOBAL, "Velocity_avg", &Velocity_avg_old);
        nc_get_att_double(ncID_UH, NC_GLOBAL, "Velocity_max", &Velocity_max_old);
        nc_get_att_double(ncID_UH, NC_GLOBAL, "Velocity_min", &Velocity_min_old);
        if (step_time == step_time_old &&
            FloatEqual(b, b_old) == 1 &&
            FloatEqual(c, c_old) == 1 &&
            FloatEqual(Velocity_avg, Velocity_avg_old) == 1 &&
            FloatEqual(Velocity_max, Velocity_max_old) == 1 &&
            FloatEqual(Velocity_min, Velocity_min_old) == 1)
        {
            new_UH = 0;
        }
        else
        {
            new_UH = 1;
        }
        nc_close(ncID_UH);
    }
    if (new_UH == 1)
    {
        printf("---------------- UH generating ----------------\n");
        printf("UH parameters:\n");
        printf("* STEP_TIME: %d\n", step_time);
        printf("* b: %.2f\n* c: %.2f\n* Velocity_avg: %.1f\n* Velocity_max: %.1f\n* Velocity_min: %.1f\n",
               b, c, Velocity_avg, Velocity_max, Velocity_min);
        int i, j;
        int ncID_GEO;
        status_nc = nc_open(FP_GEO, NC_WRITE, &ncID_GEO);
        handle_error(status_nc, FP_GEO);

        ST_Header GEO_header;
        int varID_FDR, varID_FAC, varID_DEM, varID_OUTLET;
        int varID_lon, varID_lat;
        int *data_DEM;
        int *data_FDR;
        int *data_FAC;
        int *data_OUTLET;
        double *data_lon;
        double *data_lat;
        int cellsize_m;
        int cell_counts_total;
        nc_get_att_int(ncID_GEO, NC_GLOBAL, "ncols", &GEO_header.ncols);
        nc_get_att_int(ncID_GEO, NC_GLOBAL, "nrows", &GEO_header.nrows);
        nc_get_att_int(ncID_GEO, NC_GLOBAL, "cellsize_m", &cellsize_m);
        // printf("ncols: %d\nnrows: %d\n", GEO_header.ncols, GEO_header.nrows);
        cell_counts_total = GEO_header.ncols * GEO_header.nrows;
        nc_inq_varid(ncID_GEO, "DEM", &varID_DEM);
        nc_inq_varid(ncID_GEO, "FDR", &varID_FDR);
        nc_inq_varid(ncID_GEO, "FAC", &varID_FAC);
        nc_inq_varid(ncID_GEO, "OUTLET", &varID_OUTLET);
        nc_inq_varid(ncID_GEO, "lon", &varID_lon);
        nc_inq_varid(ncID_GEO, "lat", &varID_lat);

        data_DEM = (int *)malloc(sizeof(int) * cell_counts_total);
        data_FDR = (int *)malloc(sizeof(int) * cell_counts_total);
        data_FAC = (int *)malloc(sizeof(int) * cell_counts_total);
        data_OUTLET = (int *)malloc(sizeof(int) * cell_counts_total);
        data_lon = (double *)malloc(sizeof(double) * GEO_header.ncols);
        data_lat = (double *)malloc(sizeof(double) * GEO_header.nrows);

        nc_get_var_int(ncID_GEO, varID_DEM, data_DEM);
        nc_get_var_int(ncID_GEO, varID_FDR, data_FDR);
        nc_get_var_int(ncID_GEO, varID_FAC, data_FAC);
        nc_get_var_int(ncID_GEO, varID_OUTLET, data_OUTLET);
        nc_get_var_double(ncID_GEO, varID_lon, data_lon);
        nc_get_var_double(ncID_GEO, varID_lat, data_lat);

        nc_get_att_int(ncID_GEO, varID_DEM, "NODATA_value", &GEO_header.NODATA_value);
        
        /****** UH parameters ***/
        double *data_Slope;
        double *data_FlowDistance;
        data_Slope = (double *)malloc(sizeof(double) * cell_counts_total);
        data_FlowDistance = (double *)malloc(sizeof(double) * cell_counts_total);
        Grid_Slope(data_DEM, data_FDR, data_Slope, data_FlowDistance,
                   GEO_header.ncols,
                   GEO_header.nrows,
                   GEO_header.NODATA_value,
                   cellsize_m);

        double *data_SlopeArea;
        data_SlopeArea = (double *)malloc(sizeof(double) * cell_counts_total);
        double slope_area_avg;
        Grid_SlopeArea(data_FAC, data_Slope, data_SlopeArea,
                       &slope_area_avg, b, c,
                       GEO_header.ncols,
                       GEO_header.nrows,
                       GEO_header.NODATA_value,
                       cellsize_m);
        printf("* average of slope_area term: %.3f\n", slope_area_avg);

        double *data_V;
        data_V = (double *)malloc(sizeof(double) * cell_counts_total);
        Grid_Velocity(data_FAC, data_SlopeArea, slope_area_avg, data_V,
                      Velocity_avg, Velocity_max, Velocity_min,
                      GEO_header.ncols,
                      GEO_header.nrows,
                      GEO_header.NODATA_value);

        int outlet_count = 0;
        int outlet_index_row[MAX_OUTLETS];
        int outlet_index_col[MAX_OUTLETS];
        Grid_Outlets(
            data_OUTLET,
            outlet_index_row,
            outlet_index_col,
            &outlet_count,
            GEO_header.ncols,
            GEO_header.nrows,
            GEO_header.NODATA_value);
        printf("* the number of outlets: %d\n", outlet_count);
        printf("* %5s%5s%5s\n", "outlet", "row", "col");
        for (size_t c = 0; c < outlet_count; c++)
        {
            printf("* %5d%5d%5d\n", c, outlet_index_row[c], outlet_index_col[c]);
        }

        /********** write data to NetCDF ***********/
        // int ncID_UH;
        int dimID_lon, dimID_lat, dimID_time;
        int varID_UH, varID_Slope, varID_FlowDistance, varID_SlopeArea, varID_V, varID_FlowTime, varID_OutletMask;
        nc_create(FP_UH, NC_CLOBBER, &ncID_UH);

        nc_def_dim(ncID_UH, "lon", GEO_header.ncols, &dimID_lon);
        nc_def_dim(ncID_UH, "lat", GEO_header.nrows, &dimID_lat);
        nc_def_dim(ncID_UH, "time", NC_UNLIMITED, &dimID_time);
        int dims[3];
        dims[0] = dimID_time;
        dims[1] = dimID_lat;
        dims[2] = dimID_lon;

        nc_def_var(ncID_UH, "lon", NC_DOUBLE, 1, &dimID_lon, &varID_lon);
        nc_def_var(ncID_UH, "lat", NC_DOUBLE, 1, &dimID_lat, &varID_lat);
        // nc_def_var(ncID_UH, "time", NC_INT, 1, &dimID_time, &varID_time);
        nc_def_var(ncID_UH, "Slope", NC_DOUBLE, 2, dims + 1, &varID_Slope);
        nc_def_var(ncID_UH, "FlowDistance", NC_DOUBLE, 2, dims + 1, &varID_FlowDistance);
        nc_def_var(ncID_UH, "SlopeArea", NC_DOUBLE, 2, dims + 1, &varID_SlopeArea);
        nc_def_var(ncID_UH, "Velocity", NC_DOUBLE, 2, dims + 1, &varID_V);

        nc_put_att_text(ncID_UH, varID_FlowDistance, "units", 40L, "m");
        nc_put_att_text(ncID_UH, varID_V, "units", 40L, "m/h");
        copy_global_attributes(ncID_GEO, ncID_UH);

        nc_put_att_int(ncID_UH, NC_GLOBAL, "NODATA_value", NC_INT, 1, &GEO_header.NODATA_value);
        nc_put_att_int(ncID_UH, NC_GLOBAL, "outlet_count", NC_INT, 1, &outlet_count);
        nc_put_att_int(ncID_UH, NC_GLOBAL, "STEP_TIME", NC_INT, 1, &step_time);
        nc_put_att_double(ncID_UH, NC_GLOBAL, "b", NC_DOUBLE, 1, &b);
        nc_put_att_double(ncID_UH, NC_GLOBAL, "c", NC_DOUBLE, 1, &c);
        nc_put_att_double(ncID_UH, NC_GLOBAL, "Velocity_avg", NC_DOUBLE, 1, &Velocity_avg);
        nc_put_att_double(ncID_UH, NC_GLOBAL, "Velocity_max", NC_DOUBLE, 1, &Velocity_max);
        nc_put_att_double(ncID_UH, NC_GLOBAL, "Velocity_min", NC_DOUBLE, 1, &Velocity_min);

        nc_enddef(ncID_UH);
        nc_put_var_double(ncID_UH, varID_Slope, data_Slope);
        nc_put_var_double(ncID_UH, varID_FlowDistance, data_FlowDistance);
        nc_put_var_double(ncID_UH, varID_SlopeArea, data_SlopeArea);
        nc_put_var_double(ncID_UH, varID_V, data_V);

        nc_put_var_double(ncID_UH, varID_lon, data_lon);
        nc_put_var_double(ncID_UH, varID_lat, data_lat);

        for (size_t c = 0; c < outlet_count; c++)
        {
            int *data_Mask;
            data_Mask = (int *)malloc(sizeof(int) * cell_counts_total);
            Grid_OutletMask(outlet_index_row[c], outlet_index_col[c],
                            data_FDR,
                            data_Mask,
                            GEO_header.ncols,
                            GEO_header.nrows,
                            GEO_header.NODATA_value);

            double *data_FlowTime;
            data_FlowTime = (double *)malloc(sizeof(double) * cell_counts_total);

            Grid_FlowTime(data_Mask, data_FDR, data_V, data_FlowDistance, data_FlowTime,
                          outlet_index_row[c], // int outlet_index_row,
                          outlet_index_col[c], // int outlet_index_col,
                          GEO_header.ncols,
                          GEO_header.nrows,
                          GEO_header.NODATA_value);

            double *data_UH;
            int time_steps;
            int step_time = 24; // 1 hour
            double beta = 0.5;
            Grid_UH(data_Mask, data_FlowTime, &data_UH,
                    &time_steps, beta, step_time,
                    GEO_header.ncols,
                    GEO_header.nrows,
                    GEO_header.NODATA_value);

            printf("* total time steps in UH%d: %d\n", c, time_steps);

            char varUH_name[40] = "UH";
            char varFlowTime_name[40] = "FlowTime";
            char varOutletMask_name[40] = "OutletMask";
            char numberString[4];
            sprintf(numberString, "%d", c);
            strcat(varUH_name, numberString);
            strcat(varFlowTime_name, numberString);
            strcat(varOutletMask_name, numberString);

            nc_redef(ncID_UH);
            nc_def_var(ncID_UH, varFlowTime_name, NC_DOUBLE, 2, dims + 1, &varID_FlowTime);
            nc_put_att_text(ncID_UH, varID_FlowTime, "units", 40L, "h");
            
            nc_def_var(ncID_UH, varOutletMask_name, NC_INT, 2, dims + 1, &varID_OutletMask);
            nc_put_att_int(ncID_UH, varID_OutletMask, "mask_value", NC_INT, 1, (int[]){1});

            nc_def_var(ncID_UH, varUH_name, NC_DOUBLE, 3, dims, &varID_UH);
            nc_put_att_text(ncID_UH, varID_UH, "units", 40L, "h-1");
            nc_put_att_int(ncID_UH, varID_UH, "step_time", NC_INT, 1, &step_time);
            nc_put_att_int(ncID_UH, varID_UH, "varID", NC_INT, 1, &varID_UH);
            nc_put_att_int(ncID_UH, varID_UH, "UH_steps", NC_INT, 1, &time_steps);
            nc_put_att_int(ncID_UH, varID_UH, "outlet_index_row", NC_INT, 1, outlet_index_row + c);
            nc_put_att_int(ncID_UH, varID_UH, "outlet_index_col", NC_INT, 1, outlet_index_col + c);

            nc_enddef(ncID_UH); // end of define mode

            /***************
             * write data to nc file
             */
            nc_put_var_double(ncID_UH, varID_FlowTime, data_FlowTime);
            nc_put_var_int(ncID_UH, varID_OutletMask, data_Mask);

            int out_start[3] = {0, 0, 0};
            int out_count[3];
            out_count[0] = time_steps;
            out_count[1] = GEO_header.nrows;
            out_count[2] = GEO_header.ncols;
            status_nc = nc_put_vara_double(ncID_UH, varID_UH, out_start, out_count, data_UH);
            handle_error(status_nc, FP_UH);
            free(data_FlowTime);
            free(data_Mask);
            free(data_UH);
        }

        nc_close(ncID_GEO);
        nc_close(ncID_UH);
        
        free(data_DEM); free(data_FDR); free(data_FAC); free(data_OUTLET);
        free(data_lat); free(data_lon);
        free(data_FlowDistance); free(data_Slope); free(data_SlopeArea); free(data_V);
        printf("--------------- UH generation: DONE! -------------\n");
    }
    else
    {
        printf("UH already exists, no changing parameters!\n");
    }
}


int FloatEqual(
    double x1,
    double x2)
{
    /*************
     * compare two float numbers, 
     * 1: close enough, address that two numbers equal each other.
     * 0: not close enough
    */
    double d;
    int result;
    d = x1 - x2;
    if (d < 0)
    {
        d = -d;
    }
    
    if (d < 0.0001)
    {
        result = 1;
    }
    else
    {
        result = 0;
    }
    return result;
}
