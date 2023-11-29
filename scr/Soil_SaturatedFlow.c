
/*
 * SUMMARY:      Soil_SaturatedFlow.c
 * USAGE:        Calculate the water movement in saturated soil zone
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Nov-2023
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
 * double Soil_Moisture          - moisture (water content) in the soil layer
 * double Soil_Porosity          - soil porosity
 * double Soil_Conduct_Sat       - soil vertical saturated hydraulic conductivity, [m/h]
 * double Soil_PoreSize_index    - the pore size distribution index
 * int step_time                 - time step, [h] 
 * double Soil_Bubbling_pres     - soil bubbling pressure, [m]
 * double Soil_desorption        - water desorption from soil layer, [m]
 * 
 * REFERENCEs:
 * 
 * 
 * 
******************************************************************************/


#include <math.h>
#include <Soil_SaturatedFlow.h>

