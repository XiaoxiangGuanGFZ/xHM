/*
 * SUMMARY:      Route_UH.c
 * USAGE:        Calculate overland runoff routing using Unit Hydrograph (UH) 
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  Calculate overland runoff using UH, over a gridded surface. 
 *               the UH is derived based on topography (DEM). 
 * DESCRIP-END.
 * FUNCTIONS:    Grid_Slope(), Grid_SlopeArea(), Grid_Velocity()
 *                  Grid_FlowTime(), Grid_UH(), Grid_Outlets()
 * 
 * COMMENTS:
 * - Grid_Slope():          derive the slope from DEM and flow direction (D8)
 * - Grid_SlopeArea():      compute the slope-area term for each grid cell
 * - Grid_Velocity():       assign flow velocity to each grid cell
 * - Grid_FlowTime():       compute the flow time of each grid to the outlet
 * - Grid_UH():             generate UH for each grid cell
 * - Grid_Outlets():        extract the number and coordinates (row and col index) of outlets
 * - Grid_OutletMask():     extract the mask (the upstream region) of an outlet (based on coordinates)
 * 
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

#include "GEO_ST.h"
#include "Route_UH.h"

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
    printf("Grid_Grid_Velocity: done!\n");
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
    printf("Grid_UH: done!\n");
}
