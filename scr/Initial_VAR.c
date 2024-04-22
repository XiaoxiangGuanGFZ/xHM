/*
 * SUMMARY:      Initial_VAR.c
 * USAGE:        intialize the variables structure
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Jan-2024
 * DESCRIPTION:  intialize the structures and assign the initial values 
 *               for the members
 * 
 * DESCRIP-END.
 * FUNCTIONS:    Initialize_RADIA();Initialize_ET();
 *               Initialize_SOIL();
 * 
 * COMMENTS:
 * see the details and member explanation in "HM_ST.h"
 * 
 * References:
 * 
 * 
*/

#include <stdlib.h>
#include "HM_ST.h"
#include "Initial_VAR.h"

void Initialize_RADIA(
    CELL_VAR_RADIA *st)
{
    for (size_t i = 0; i < sizeof(st) / sizeof(double); i++)
    {
        double *currentMember = ((double *)st) + i;
        *currentMember = 0.0;
    }
}

void Initialize_ET(
    CELL_VAR_ET *st)
{
    for (size_t i = 0; i < sizeof(st) / sizeof(double); i++)
    {
        double *currentMember = ((double *)st) + i;
        *currentMember = 0.0;
    }
    st->Interception_o = 0.0;
    st->Interception_u = 0.0;
}

void Initialize_SOIL(
    CELL_VAR_SOIL *st)
{
    st->SW_Infiltration = 0.0;
    st->SW_Percolation_Lower = 0.0;
    st->SW_Percolation_Upper = 0.0;
    st->SW_SR_Infil = 0.0;
    st->SW_SR_Satur = 0.0;
    st->SM_Lower = 0.4;
    st->SM_Upper = 0.4;
}


void Initialize_Soil_Satur(
    CELL_VAR_SOIL *data_SOIL,
    int *data_DEM,
    int NODATA_value,
    int ncols,
    int nrows)
{
    int cell_counts_total;
    cell_counts_total = ncols * nrows;
    int index_geo;
    int index_geo_neighbor[8];
    int DEM8_max;

    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
        {
            index_geo = i * ncols + j;

            if (*(data_DEM + index_geo) != NODATA_value)
            {
                (data_SOIL + index_geo)->z = 0.05;
                (data_SOIL + index_geo)->Qin = 0.0;
                (data_SOIL + index_geo)->Qout = 0.0;
                (data_SOIL + index_geo)->SW_rf = 0.0;
                (data_SOIL + index_geo)->SW_rise_lower = 0.0;
                (data_SOIL + index_geo)->SW_rise_upper = 0.0;
                // the 8 neighbos of the central cell
                index_geo_neighbor[0] = (i - 1) * ncols + j - 1;
                index_geo_neighbor[1] = (i - 1) * ncols + j;
                index_geo_neighbor[2] = (i - 1) * ncols + j + 1;
                index_geo_neighbor[3] = i * ncols + j + 1;
                index_geo_neighbor[4] = (i + 1) * ncols + j + 1;
                index_geo_neighbor[5] = (i + 1) * ncols + j;
                index_geo_neighbor[6] = (i + 1) * ncols + j - 1;
                index_geo_neighbor[7] = i * ncols + j - 1;

                for (size_t k = 0; k < 8; k++)
                {
                    // initialize the neighbor: all 1
                    (data_SOIL + index_geo)->neighbor[k] = 1;
                }
                // identify the boundary cells
                if (i == 0)
                {
                    (data_SOIL + index_geo)->neighbor[0] = 0;
                    (data_SOIL + index_geo)->neighbor[1] = 0;
                    (data_SOIL + index_geo)->neighbor[2] = 0;
                }
                else if (i == (nrows - 1))
                {
                    (data_SOIL + index_geo)->neighbor[4] = 0;
                    (data_SOIL + index_geo)->neighbor[5] = 0;
                    (data_SOIL + index_geo)->neighbor[6] = 0;
                }
                if (j == 0)
                {
                    (data_SOIL + index_geo)->neighbor[0] = 0;
                    (data_SOIL + index_geo)->neighbor[6] = 0;
                    (data_SOIL + index_geo)->neighbor[7] = 0;
                }
                else if (j == (ncols - 1))
                {
                    (data_SOIL + index_geo)->neighbor[2] = 0;
                    (data_SOIL + index_geo)->neighbor[3] = 0;
                    (data_SOIL + index_geo)->neighbor[4] = 0;
                }
                
                for (size_t k = 0; k < 8; k++)
                {
                    // cell with NODATA_value neighbors
                    if ((data_SOIL + index_geo)->neighbor[k] == 1)
                    {
                        if (*(data_DEM + index_geo_neighbor[k]) == NODATA_value)
                        {
                            (data_SOIL + index_geo)->neighbor[k] = 0;
                        }
                    }
                }

                /**************
                 * highest cell; the maximum of cell height
                 * among all the valid neighboring cells
                 * */ 
                DEM8_max = *(data_DEM + index_geo);
                for (size_t k = 0; k < 8; k++)
                {
                    if ((data_SOIL + index_geo)->neighbor[k] == 1)
                    {
                        if (DEM8_max < *(data_DEM + index_geo_neighbor[k]))
                        {
                            DEM8_max = *(data_DEM + index_geo_neighbor[k]);
                        }
                    }
                }
                // calculate the offset of height (DEM)
                (data_SOIL + index_geo)->z_offset = DEM8_max - *(data_DEM + index_geo);
                for (size_t k = 0; k < 8; k++) 
                {
                    if ((data_SOIL + index_geo)->neighbor[k] == 1)
                    {
                        (data_SOIL + index_geo)->z_offset_neighbor[k] = DEM8_max - *(data_DEM + index_geo_neighbor[k]);
                        (data_SOIL + index_geo)->q[k] = 0.0;
                    } else
                    {
                        (data_SOIL + index_geo)->z_offset_neighbor[k] = NODATA_value;
                        (data_SOIL + index_geo)->q[k] = (double)NODATA_value;
                    }
                }
            }
            else
            {
                // NODATA_value: outside the mask
                (data_SOIL + index_geo)->z = (double)NODATA_value;
                (data_SOIL + index_geo)->Qin = (double)NODATA_value;
                (data_SOIL + index_geo)->Qout = (double)NODATA_value;
                (data_SOIL + index_geo)->z_offset = NODATA_value;
                (data_SOIL + index_geo)->SW_rf = (double)NODATA_value;
                (data_SOIL + index_geo)->SW_rise_lower = (double)NODATA_value;
                (data_SOIL + index_geo)->SW_rise_upper = (double)NODATA_value;
                for (size_t k = 0; k < 8; k++)
                {
                    (data_SOIL + index_geo)->neighbor[k] = 0;
                    (data_SOIL + index_geo)->z_offset_neighbor[k] = NODATA_value;
                    (data_SOIL + index_geo)->q[k] = (double)NODATA_value;
                }
            }
        }
    }
}
