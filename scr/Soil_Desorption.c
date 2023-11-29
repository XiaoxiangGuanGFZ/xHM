
/*
 * SUMMARY:      Soil_Desorption.c
 * USAGE:        Calculate the desorption in unsaturated soil zone
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Nov-2023
 * DESCRIPTION:  Calculate unsaturated soil zone desorption
 * DESCRIP-END.
 * FUNCTIONS:    
 *               Soil_Desorption()
 * COMMENTS:
 * 
 * 
 */


/****************************************************************************
 * VARIABLEs:
 * double Soil_Moisture          - moisture (water content) in the soil layer
 * double Soil_Porosity          - soil porosity
 * double Soil_Conduct_Sat       - soil vertical saturated hydraulic conductivity, [m/h]
 * double Soil_PoreSize_index    - the pore size distribution index
 * int step_time                 - time step, [h] 
 * double Soil_Bubbling_pres     - soil bubbling pressure, [m]
 * double Soil_desorption        - water desorption from soil layer, [m]
 * 
 * REFERENCEs:
 * Entekhabi, D., and P.S. Eagleson, Land surface hydrology 
 *      parameterization for atmospheric general circulation 
 *      models: Inclusion of subgrid scale spatial variability 
 *      and screening with a simple climate model, Rep. 325, 
 *      Ralph M. Parsons Lab., 195 pp., Mass. Inst. of Technol., Cambridge, 1989.
 * 
 * 
******************************************************************************/

#include <math.h>
#include "Soil_Desorption.h"


double Soil_Desorption(
    double Soil_Moisture,
    double Soil_Conduct_Sat,
    double Soil_PoreSize_index,
    double Soil_Porosity,
    double Soil_Bubbling_pres,
    int step_time
)
{
    /************
     * calculate the soil desorption, 
     * Entekhabi and Eagleson [1989]
    */
    double Sorptivity;
    double Soil_desorption;
    Sorptivity = pow(
        8 * Soil_Porosity * Soil_Conduct_Sat * Soil_Bubbling_pres /
        (3 * (1 + 3 * Soil_PoreSize_index) * (1 + 4 * Soil_PoreSize_index)),
        0.5
    ) * pow(
        Soil_Moisture / Soil_Porosity,
        (1 / (2 * Soil_PoreSize_index) + 2)
    );
    Soil_desorption = Sorptivity * pow((double) step_time, 0.5);
    return Soil_desorption;
}

