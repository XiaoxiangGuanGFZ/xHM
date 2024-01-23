/*
 * SUMMARY:      Route_Channel.c
 * USAGE:        Calculate the flow routing in the river channel (reach)
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  Calculate the flow routing process in the 
 *               river channels (segments or reaches), with a 
 *               linear storage method
 * DESCRIP-END.
 * FUNCTIONS:    
 * 
 * COMMENTS:
 * 
 * 
 * REFERENCES:
 *  
 * 
 */

/******************************************************************
 * VARIABLES:
 * double Qc               - the (subsurface) water flow between grid cell and the river channel, [m3/h]
 * double Q                - the streamflow of the river reach, [m3/h]
 * double V                - the water volume of the river reach, [m3]
 * double k                - storage parameter: equal to the inverse of the average residence time, [1/h]
 * int step_time           - time step, interval, [h]
 * int *data_STR           - pointing to the 2D stream (STR) array
 * 
******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "HM_ST.h"
#include "Route_Channel.h"

void Channel_Routing(
    double *Qin,
    double *Qout,
    double *V,
    double Qc,
    double k,
    int step_time)
{
    double Vin;
    Vin = *V;
    *V = (*Qin + Qc) / k + (Vin - (*Qin + Qc) / k) * exp(- k * step_time);
    *Qout = (*Qin + Qc) - (*V - Vin) / step_time;
}

void Initialize_STREAM(
    CELL_VAR_STREAM **data_STREAM,
    int *data_STR,
    int *data_FDR,
    double ROUTE_CHANNEL_k,
    int NODATA_value,
    int ncols,
    int nrows)
{
    int dir;
    int index_geo;
    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
        {
            index_geo = i * ncols + j;
            if (*(data_STR + index_geo) == NODATA_value)
            {
                (*data_STREAM + index_geo)->k = NODATA_value;
                (*data_STREAM + index_geo)->Qin = NODATA_value;
                (*data_STREAM + index_geo)->Qout = NODATA_value;
                (*data_STREAM + index_geo)->Qc = NODATA_value;
                (*data_STREAM + index_geo)->V = NODATA_value;
                (*data_STREAM + index_geo)->next_row = NODATA_value;
                (*data_STREAM + index_geo)->next_col = NODATA_value;
            }
            else
            {
                (*data_STREAM + index_geo)->Qin = 0;
                (*data_STREAM + index_geo)->Qout = 0;
                (*data_STREAM + index_geo)->Qc = 0;
                (*data_STREAM + index_geo)->V = 0;
                if (*(data_STR + index_geo) == 0)
                {
                    (*data_STREAM + index_geo)->k = NODATA_value;
                    (*data_STREAM + index_geo)->next_row = NODATA_value;
                    (*data_STREAM + index_geo)->next_col = NODATA_value;
                }
                else if (*(data_STR + index_geo) == 1)
                {
                    // there is channel reach in this grid cell
                    (*data_STREAM + index_geo)->k = ROUTE_CHANNEL_k;

                    dir = *(data_FDR + index_geo);
                    switch (dir)
                    {
                    case 1:
                    {
                        (*data_STREAM + index_geo)->next_row = i;
                        (*data_STREAM + index_geo)->next_col = j + 1;
                        break;
                    }
                    case 2:
                    {
                        (*data_STREAM + index_geo)->next_row = i + 1;
                        (*data_STREAM + index_geo)->next_col = j + 1;
                        break;
                    }
                    case 4:
                    {
                        (*data_STREAM + index_geo)->next_row = i + 1;
                        (*data_STREAM + index_geo)->next_col = j;
                        break;
                    }
                    case 8:
                    {
                        (*data_STREAM + index_geo)->next_row = i + 1;
                        (*data_STREAM + index_geo)->next_col = j - 1;
                        break;
                    }
                    case 16:
                    {
                        (*data_STREAM + index_geo)->next_row = i;
                        (*data_STREAM + index_geo)->next_col = j - 1;
                        break;
                    }
                    case 32:
                    {
                        (*data_STREAM + index_geo)->next_row = i - 1;
                        (*data_STREAM + index_geo)->next_col = j - 1;
                        break;
                    }
                    case 64:
                    {
                        (*data_STREAM + index_geo)->next_row = i - 1;
                        (*data_STREAM + index_geo)->next_col = j;
                        break;
                    }
                    case 128:
                    {
                        (*data_STREAM + index_geo)->next_row = i - 1;
                        (*data_STREAM + index_geo)->next_col = j + 1;
                        break;
                    }
                    default:
                    {
                        printf("Unrecognized flow direction field value at the location row: %d, col: %d!", i + 1, j + 1);
                        exit(0);
                    }
                    }
                }
            }
        }
    }
}

void Channel_Network_Routing(
    CELL_VAR_STREAM **data_STREAM,
    int *data_STR,
    int ncols,
    int nrows,
    int step_time)
{
    int index_geo;

    /* initialize Qin for each running step */
    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
        {
            index_geo = i * ncols + j;
            if (*(data_STR + index_geo) == 1)
            {
                (*data_STREAM + index_geo)->Qin = 0.0;
            }
        }
    }
    /* update Qin */
    int index_geo_next;
    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
        {
            index_geo = i * ncols + j;
            if (*(data_STR + index_geo) == 1)
            {
                index_geo_next = (*data_STREAM + index_geo)->next_row * ncols + (*data_STREAM + index_geo)->next_col;
                (*data_STREAM + index_geo_next)->Qin += (*data_STREAM + index_geo)->Qout;
            }
        }
    }
    /* route Qout */
    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
        {
            index_geo = i * ncols + j;
            if (*(data_STR + index_geo) == 1)
            {
                Channel_Routing(
                    &((*data_STREAM + index_geo)->Qin),
                    &((*data_STREAM + index_geo)->Qout),
                    &((*data_STREAM + index_geo)->V),
                    (*data_STREAM + index_geo)->Qc,
                    (*data_STREAM + index_geo)->k,
                    step_time);
            }
        }
    }
}
