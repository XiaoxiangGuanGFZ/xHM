
/*
 * SUMMARY:      Soil_Percolation.c
 * USAGE:        Calculate the percolation in unsaturated soil zone
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Nov-2023
 * DESCRIPTION:  Calculate unsaturated soil zone percolation
 * DESCRIP-END.
 * FUNCTIONS:    
 *               Percolation(); Soil_Hydro_Conductivity()
 * COMMENTS:
 * two-layer soil conceptualization: upper layer and lower layer.
 * 
 * 
 */


/******************************************************************************
 * VARIABLEs:
 * double Soil_Moisture          - moisture (water content) in the soil layer
 * double Percolation_in         - water content percolated into this soil layer
 * double Percolation_out        - water content percolated from this soil layer
 * double Soil_layer_thickness   - soil layer thickness, [m]
 * double Soil_Porosity          - soil porosity
 * double Soil_Residual          - residual soil moisture content
 * double Soil_Conduct_Sat       - soil vertical saturated hydraulic conductivity, [m/h]
 * double Soil_PoreSize_index    - the pore size distribution index
 * int step_time                 - time step, [h] 
 * double Soil_Conduct           - soil vertical unsaturated hydraulic conductivity, [m/h]
 * 
 * 
 * REFERENCEs:
 * Brooks, R. H., and A. T. Corey, 
 *      Hydraulic properties of porous media, Hydrol. 
 *      Pap., 3, Colo. State Univ., Fort Collins, 1964.
 * 
 * 
******************************************************************************/

#include <math.h>
#include "Soil_Percolation.h"


double Percolation(
    double Soil_Moisture,
    double Percolation_in,
    double Soil_layer_thickness,
    double Soil_Porosity,
    double Soil_Residual,
    double Soil_Conduct_Sat,
    double Soil_PoreSize_index,
    int step_time 
)
{
    double Percolation_out;
    double Soil_Conduct, Soil_Conduct_end;
    Soil_Conduct = Soil_Hydro_Conductivity(
        Soil_Moisture, Soil_Porosity, Soil_Residual, 
        Soil_Conduct_Sat, Soil_PoreSize_index
    );
    Soil_Conduct_end = Soil_Hydro_Conductivity(
        Soil_Moisture + Percolation_in / Soil_layer_thickness, 
        Soil_Porosity, Soil_Residual, 
        Soil_Conduct_Sat, Soil_PoreSize_index
    );
    Percolation_out = 0.5 * (Soil_Conduct_end + Soil_Conduct) * (double) step_time;

    return Percolation_out;
}

double Soil_Hydro_Conductivity(
    double Soil_Moisture,
    double Soil_Porosity,
    double Soil_Residual,
    double Soil_Conduct_Sat,
    double Soil_PoreSize_index
)
{
    /*********
     * calculate the soil vertical 
     * unsaturated hydraulic conductivity, [m/h]
     *  [Brooks and Corey, 1964]
    */
    double Soil_Conduct;
    Soil_Conduct = Soil_Conduct_Sat * pow(
        (Soil_Moisture - Soil_Residual) / (Soil_Porosity - Soil_Residual),
        (2 / Soil_PoreSize_index + 3)
    );
    return Soil_Conduct;
}

