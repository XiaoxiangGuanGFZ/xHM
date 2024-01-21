
/*
 * SUMMARY:      Soil_SaturatedFlow.c
 * USAGE:        Calculate the water movement in saturated soil zone
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Jan-2024
 * DESCRIPTION:  Calculate saturated water movement
 * DESCRIP-END.
 * FUNCTIONS:    
 *               ()
 * COMMENTS:
 * 
 * 
 */

/****************************************************************************
 * VARIABLEs:
 * double Soil_Thickness                - soil thickness, [m]
 * double Soil_Porosity                 - soil porosity, [%Vol]
 * double Soil_Conduct_Sat_Lateral      - soil surface lateral saturated hydraulic conductivity, [m/h]
 * double Cell_WT_z                     - depth (water table) into the soil profile (positive downward), [m]
 * double *Cell_WT                      - pointer to an array of water table depth in 8 adjacent cells, [m]
 * double step_space                    - grid cell size, [m]
 * double width[8]                      - the width between central cell with 8 neighbours, [m]
 * double *Cell_q                       - pointer to an array of outflow from the cell to 8 adjacent cells, [m3/h]
 * double *Qout                         - total outflow from the cell, [m3/h]
 * double *Qin                          - total inflow to the cell, [m3/h]
 * double n                             - the local power law exponent, decaying coefficient of lateral hydraulic conductivity
 * double *F                            - pointer to an array of outflow fractions to 8 directions
 *
 *
 * REFERENCEs:
 *
 *
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Constants.h"
#include "HM_ST.h"
#include "Lookup_SoilLib.h"
#include "Soil_SaturatedFlow.h"

double Soil_Satu_grad
(
    /*******
     *  \gamma_{i,j,k}
     * 
    */
    double Soil_Conduct_Sat_Lateral,
    double Soil_Thickness,
    double n,
    double Cell_WT_z,
    double Cell_WT_k //,
    // double width 
)
{
    // double slope;
    double gamma;
    if (
        Cell_WT_k <= Cell_WT_z
    ){
        // slope = 0.0; 
        gamma = 0.0;
    } else {
        // slope = (Cell_WT_z - Cell_WT_k) / width;  // slope < 0.0
        // gamma = - slope * (width * Soil_Conduct_Sat_Lateral * Soil_Thickness / n);
        gamma = - (Cell_WT_z - Cell_WT_k) * (Soil_Conduct_Sat_Lateral * Soil_Thickness / n);
    }
    return gamma;
}

void Soil_Satu_Outflow(
    double Cell_WT_z,
    int z_offset,
    int *neighbor,
    double *Cell_WT_rf,
    double *Cell_q,
    double *Qout,
    double Soil_Conduct_Sat_Lateral,
    double Soil_Thickness,
    double n
)
{
    /******************************************
     * calculate the outflow from a grid cell
     * into 8 other (valid) directions
    */
    double width[8] = {1.41, 1, 1.41, 1, 1.41, 1, 1.41, 1};
    double slope[8];
    double gamma[8];
    double gamma_sum = 0.0;
    double F[8];
    *Qout = 0.0;
    double h;
    h = pow(1 - Cell_WT_z/Soil_Thickness, n);  // soil moisture deficit
    int k = 0;
    for (k = 0; k < 8; k++)
    {
        if (neighbor[k] == 1)
        {
            *(gamma + k) = Soil_Satu_grad(
                Soil_Conduct_Sat_Lateral,
                Soil_Thickness,
                n,
                Cell_WT_z + z_offset,
                *(Cell_WT_rf + k)
                );
            gamma_sum += *(gamma + k);
            *Qout += h * *(gamma + k);
        } else {
            *(gamma + k) = 0.0;
        }
    }
    if (gamma_sum <= 0.0)
    {
        for (k = 0; k < 8; k++)
        {
            *(F + k) = 0.0;
            *(Cell_q + k) = 0.0;
        }
    } else {
        for (k = 0; k < 8; k++)
        {
            *(F + k) = *(gamma + k) / gamma_sum;
            *(Cell_q + k) = *(F + k) * *Qout;
        }
    }
}


double Soil_Satu_Stream(
    double z,
    double stream_length,
    double stream_depth,
    double stream_width,
    double Soil_Conduct_Sat_Lateral,
    double Soil_Thickness,
    double n
)
{
    /*******************************
     * A grid cell will contribute water to a stream reach
     *      when the grid cell water table rises above the streambed,
     *      namely subsurface flow intercepted by the channel at a rate of Qc;
     * Qc > 0: grid cell contributes water to stream channel: water table is higher than channel bed (stream_depth > z).
     * Qc < 0: water flows from channel to the grid cell: water table is lower than channel bed.
     *********************/
    double Trans;
    double Qc;
    Trans = Soil_Conduct_Sat_Lateral * Soil_Thickness / n * pow(1 - z/Soil_Thickness, n);
    // Qc = 2 * stream_length * (stream_depth - z) / (0.5 * stream_width) * Trans;
    Qc = 4 * stream_length * (stream_depth - z) / stream_width * Trans;
    return Qc;
}

void Soil_Satu_Move(
    int *data_DEM,
    int *data_STR,
    int *data_SOILTYPE,
    CELL_VAR_STREAM **data_STREAM,
    CELL_VAR_SOIL **data_SOIL,
    ST_SoilLib *soillib,
    ST_SoilID *soilID,
    double Soil_Thickness,
    double Soil_d1,
    double Soil_d2,
    double stream_depth,
    double stream_width,
    int NODATA_value,
    int ncols,
    int nrows,
    double step_space,
    int step_time)
{
    /******************************************
     * 
    */
    double cell_area;   // the area of the grid cell, m2
    cell_area = step_space * step_space; // total number of grid cells; size of 2D array
    int index_geo;  
    int index_geo_neighbor[8];
    double Cell_WT_rf[8];

    // direction where the rid cell receiving from other cells yielding outflow
    int in_dir[8] = {4, 5, 6, 7, 0, 1, 2, 3};
    ST_SOIL_LIB_CELL cell_soil;
    double Porosity; // depending on where is the water table, upper or lower soil layer;
    double dZ; // water table changes
    double dW; // water volume changes

    /*********************************
     * calculate the outflow from grid cell 
     * into 8 directions
     * */ 
    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
        {
            index_geo = i * ncols + j;
            if (*(data_DEM + index_geo) != NODATA_value)
            {
                Lookup_Soil_CELL(*(data_SOILTYPE + index_geo), &cell_soil, soillib, soilID);
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
                    if ((*data_SOIL + index_geo)->neighbor[k] == 1)
                    {
                        Cell_WT_rf[k] = (*data_SOIL + index_geo)->z_offset_neighbor[k] + 
                        (*data_SOIL + index_geo_neighbor[k])->z;
                    }
                    else
                    {
                        Cell_WT_rf[k] = NODATA_value;
                    }
                }
                /******************
                 * calculate the outflow (q[8]) from this cell
                 * to each direction and the total outflow, [m3/h]
                */
                Soil_Satu_Outflow(
                    (*data_SOIL + index_geo)->z,
                    (*data_SOIL + index_geo)->z_offset,
                    (*data_SOIL + index_geo)->neighbor,
                    Cell_WT_rf,
                    (*data_SOIL + index_geo)->q,
                    &((*data_SOIL + index_geo)->Qout),
                    cell_soil.Topsoil->SatHydrauCond_Lateral,
                    Soil_Thickness,
                    cell_soil.Topsoil->DecayCoeff
                    );
                printf("%f,", (*data_SOIL + index_geo)->Qout);
            }
        }
    }
    printf("\n");
    /*****************************
     * calculate the inflow of each grid cell
     */

    for (size_t i = 0; i < nrows; i++)
    {
        for (size_t j = 0; j < ncols; j++)
        {
            index_geo = i * ncols + j;
            if (*(data_DEM + index_geo) != NODATA_value)
            {
                Lookup_Soil_CELL(*(data_SOILTYPE + index_geo), &cell_soil, soillib, soilID);

                index_geo_neighbor[0] = (i - 1) * ncols + j - 1;
                index_geo_neighbor[1] = (i - 1) * ncols + j;
                index_geo_neighbor[2] = (i - 1) * ncols + j + 1;
                index_geo_neighbor[3] = i * ncols + j + 1;
                index_geo_neighbor[4] = (i + 1) * ncols + j + 1;
                index_geo_neighbor[5] = (i + 1) * ncols + j;
                index_geo_neighbor[6] = (i + 1) * ncols + j - 1;
                index_geo_neighbor[7] = i * ncols + j - 1;

                (*data_SOIL + index_geo)->Qin = 0.0;
                for (size_t k = 0; k < 8; k++)
                {
                    if ((*data_SOIL + index_geo)->neighbor[k] == 1)
                    {
                        (*data_SOIL + index_geo)->Qin += 
                            (*data_SOIL + index_geo_neighbor[k])->q[in_dir[k]]; 
                    }
                }

                if (*(data_STR + index_geo) == 1)
                {
                    /* this is a cell with river channel/stream */
                    (*data_STREAM + index_geo)->Qc = Soil_Satu_Stream(
                        (*data_SOIL + index_geo)->z,
                        step_space,
                        stream_depth,
                        stream_width,
                        cell_soil.Topsoil->SatHydrauCond_Lateral,
                        Soil_Thickness,
                        cell_soil.Topsoil->DecayCoeff);
                }
                
                /*******************
                 * update:
                 * - the underground (subsurface) water table, z
                 * - the water volume transferred vertically, SW_sf, SW_rise
                 ******/ 
                if (
                    (*data_SOIL + index_geo)->z <= Soil_d1)
                {
                    Porosity = cell_soil.Topsoil->Porosity / 100;
                }
                else
                {
                    Porosity = cell_soil.Subsoil->Porosity / 100;
                }
                dW = ((*data_SOIL + index_geo)->Qout + 
                    (*data_STREAM + index_geo)->Qc - 
                    (*data_SOIL + index_geo)->Qin) / cell_area * step_time -
                    (*data_SOIL + index_geo)->SW_Percolation_Lower;
                dZ = dW / Porosity;
                // (data_SOIL + index_geo)->z += dZ;
                (*data_SOIL + index_geo)->SW_rise_lower = 0;
                (*data_SOIL + index_geo)->SW_rise_upper = 0;
                (*data_SOIL + index_geo)->SW_rf = 0;
                if ((*data_SOIL + index_geo)->z + dZ > Soil_Thickness)
                {
                    (*data_SOIL + index_geo)->z = Soil_Thickness;
                }
                else if ((*data_SOIL + index_geo)->z + dZ < 0)
                {
                    (*data_SOIL + index_geo)->SW_rf = -(dZ + (*data_SOIL + index_geo)->z) * Porosity;
                    (*data_SOIL + index_geo)->SW_rise_upper = Porosity * (*data_SOIL + index_geo)->z;
                    (*data_SOIL + index_geo)->z = 0.0;
                }
                else
                {
                    if ((*data_SOIL + index_geo)->z > Soil_d1)
                    {
                        (*data_SOIL + index_geo)->SW_rise_lower = -dW;
                    }
                    else if ((*data_SOIL + index_geo)->z <= Soil_d1)
                    {

                        (*data_SOIL + index_geo)->SW_rise_upper = -dW;
                    }
                }
            }
        }
    }
}



