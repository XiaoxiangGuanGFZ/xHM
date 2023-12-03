
/*
 * SUMMARY:      Soil_Infiltration.c
 * USAGE:        Calculate the infiltration
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Nov-2023
 * DESCRIPTION:  Calculate the infiltration from ground surface into soil
 * DESCRIP-END.
 * FUNCTIONS:    
 * 
 * COMMENTS:
 * 
 * 
 * REFERENCES:
 * Entekhabi, D., and P.S. Eagleson, Land surface hydrology parameterization for 
 *      atmospheric general circulation models: Inclusion of subgrid scale spatial variability
 *      and screening with a simple climate model, Rep. 325, 
 *      Ralph M. Parsons Lab., 195 pp., Mass. Inst. of Technol., Cambridge, 1989.
 * 
 * 
 */

/**************************************************************
 * VARIABLES:
 * double Water_input          - water input rate above the ground, [m/h] 
 * double Soil_Moisture        - soil moisture (water content)
 * double Soil_Porosity        - soil porosity
 * double Soil_Conduct_Sat     - saturated soil hydraulic conductivity, [m/h]
 * double Air_Entry_Pres       - air-entry pressure head, [m]
 * double b                    - soil pore-size distribution parameter
 * double Infiltration         - water amount infiltrated into soil layer, [m]
 * int step_time               - time interval, [h]
 * double Tension_effective    - effective tension at the wetting front, [m]
 * 
****************************************************************/

#include <math.h>
#include "Soil_Infiltration.h"

double Soil_Infiltration(
    double Water_input,
    double Soil_Moisture,
    double Soil_Porosity,
    double Soil_Conduct_Sat,
    double Air_Entry_Pres,
    double b, 
    int step_time
)
{
    double Infiltration;
    if (
        Water_input <= Soil_Conduct_Sat
    )
    {
        /***
         * the water input rate is less than the 
         * saturated hydraulic conductivity
        */
        Infiltration = Water_input * (double) step_time;
    }
    else
    {
        double Tension_effective;
        if (Air_Entry_Pres < 0.0) {
            Air_Entry_Pres = - Air_Entry_Pres;
        }
        Tension_effective = (2 * b + 3) / (2 * b + 6) * Air_Entry_Pres;

        double Tx, Tc, Tp; 
        /****
         * time parameters:
         * Tx: a characteristic time
         * Tc: a compression time
         * Tp: the time of ponding
         * Te: an effective time
        */
        double Te;
        Tx = Tension_effective * (Soil_Porosity - Soil_Moisture) / Soil_Conduct_Sat;
        Tp = Soil_Conduct_Sat * Tension_effective * (Soil_Porosity - Soil_Moisture) / 
                Water_input / (Water_input - Soil_Conduct_Sat);
        Tc = Water_input * Tp / Soil_Conduct_Sat - 
                Tension_effective * (Soil_Porosity - Soil_Moisture) / Soil_Conduct_Sat * 
                log(
                    1 + Water_input * Tp / Tension_effective / (Soil_Porosity - Soil_Moisture)
                );
        Te = step_time - Tp + Tc;
        Infiltration = Soil_Conduct_Sat * (
            0.529 * Te + 
            0.471 * pow(
                Tx * Te + Te * Te, 0.5
            ) + 
            0.138 * Tx * (
                log(Te + Tx) - log(Tx)
            ) + 
            0.471 * Tx * (
                log(
                    Te + Tx/2 + pow(Tx * Te + Te * Te, 0.5)
                ) - log(Tx / 2)
            )
        );
    }
    
    return Infiltration;
}