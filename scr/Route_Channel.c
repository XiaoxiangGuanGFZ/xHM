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
#include <math.h>
#include "HM_ST.h"
#include "Route_Channel.h"

void Channel_Routing(
    double *Q,
    double *V,
    double Qc,
    double k,
    int step_time
)
{
    double Vin;
    double Qin;
    Vin = *V;
    Qin = *Q + Qc;
    if (Qin < 0)
    {
        *Q = 0.0;
        *V = 0.0;
    }
    else
    {
        *V = Qin / k + (*V - Qin / k) * exp(- k * step_time);
        *Q = Qin - (*V - Vin) / step_time;
    }
}


void Initialize_STREAM(
    CELL_VAR_STREAM **data_STREAM,
    int *data_STR,
    double ROUTE_CHANNEL_k,
    int NODATA_value,
    int ncols,
    int nrows)
{
    int index_geo;  
    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
        {
            index_geo = i * ncols + j;
            if (*(data_STR + index_geo) == NODATA_value)
            {
                (*data_STREAM + index_geo)->k = NODATA_value;
                (*data_STREAM + index_geo)->Q = NODATA_value;
                (*data_STREAM + index_geo)->Qc = NODATA_value;
                (*data_STREAM + index_geo)->V = NODATA_value;
            }
            else if (*(data_STR + index_geo) == 0)
            {
                (*data_STREAM + index_geo)->k = NODATA_value;
                (*data_STREAM + index_geo)->Q = 0;
                (*data_STREAM + index_geo)->Qc = 0;
                (*data_STREAM + index_geo)->V = 0;
            }
            else if (*(data_STR + index_geo) == 1)
            {
                (*data_STREAM + index_geo)->k = ROUTE_CHANNEL_k;
                (*data_STREAM + index_geo)->Q = 0.0;
                (*data_STREAM + index_geo)->Qc = 0.0;
                (*data_STREAM + index_geo)->V = 0.0;
            }
        }
    }
}

void Channel_Routing_ITER(
    CELL_VAR_STREAM **data_STREAM,
    int *data_STR,
    int ncols,
    int nrows,
    int step_time
)
{
    int index_geo;
    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
        {
            index_geo = i * ncols + j;
            if (*(data_STR + index_geo) == 1)
            {
                Channel_Routing(
                    &((*data_STREAM + index_geo)->Q),
                    &((*data_STREAM + index_geo)->V),
                    (*data_STREAM + index_geo)->Qc,
                    (*data_STREAM + index_geo)->k,
                    step_time);
            }
        }
    }
}
