
/*
 * SUMMARY:      Evaporation_soil.c
 * USAGE:        Calculate evaporation from soil (moisture)
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Nov-2023
 * DESCRIPTION:  Calculate soil evaporation
 * DESCRIP-END.
 * FUNCTIONS:    ET_soil()
 * 
 * COMMENTS:
 * two-layer soil conceptualization: upper layer and lower layer.
 * 
 * Evaporation from the upper soil layer is simulated in the absence of an understory, 
 *  otherwise soil evaporation is ignored.
 * 
 * When wet, a soil may be able to supply water to the surface 
 *  at a rate equal to or greater than the potential evaporation 
 *  demand. As soil moisture is depleted, the rate of delivery falls 
 *  below the potential evaporation rate. At this and lower moisture states, 
 *  the evaporation rate is soil-controlled and is a nonlinear 
 *  function of the soil moisture content. 
 * 
 * The soil desorption (Soil_Fe) is estimated by 
 *      Soil_Desorption() in Soil_Desorption.c.
 *      It is a function of soil texture and moisture
 */



#include <math.h>
#include "Evaporation_soil.h"

double ET_soil(
    double ET_soil_pot, /* potential evaporation rate, [m/h] */ 
    double Soil_Fe      /* soil desorptivity rate, [m/h] */  
)
{
    /****
     * Soil_Fe: 
     * the soil desorptivity, determined by the rate at 
     *      which the soil can deliver water to the surface. 
     *      Desorptivity is a function of soil type and moisture 
     *      conditions in the upper soil zone.
    */
    double ET_s;
    double epsilon = 0.000001; // Adjust according to your precision requirements
    if ((ET_soil_pot - Soil_Fe) <= epsilon)
    {
        ET_s = ET_soil_pot;
    }
    else
    {
        ET_s = Soil_Fe;
    }

    // ET_s = min(ET_soil_pot, Soil_Fe);
    return ET_s;  // m/h
}
