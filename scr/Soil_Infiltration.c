
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
 * the soil (unstaturated) hydraulic conductivity is derived 
 * from Brooks and Corey 1966.
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

#include <stdio.h>
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
        Infiltration = Water_input * step_time;
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
        if (Te < 0.0)
        {
            // if (Water_input <= Soil_Conduct_Sat + 0.00082)
            // {
            //     Infiltration = Soil_Conduct_Sat * ((double)step_time);
            // }
            /***************************
             * we found that for the clay soil type, the ponding time could be
             * extraordinarily long, longer than step_time + Tc, when the water input is
             * very close to the saturated hydraulic
             * conductivity (of clay), therefore we increased infiltration threshold.
             */
            Infiltration = Soil_Conduct_Sat * step_time;
        }
        else
        {
            Infiltration = Soil_Conduct_Sat * (0.529 * Te +
                                               0.471 * pow(
                                                           Tx * Te + Te * Te, 0.5) +
                                               0.138 * Tx * (log(Te + Tx) - log(Tx)) +
                                               0.471 * Tx * (log(Te + Tx / 2 + pow(Tx * Te + Te * Te, 0.5)) - log(Tx / 2)));
            // printf("Infiltration: %f\n", Infiltration);
            if ((int)(Infiltration * 1000) <= 0.0)
            {
                Infiltration = Soil_Conduct_Sat * step_time;
            }
        }


    }
    return Infiltration;
}

// int main(int argc, char const *argv[])
// {
//     double Water_input[18] = {0.004, 0.0094, 0.01, 0.015, 0.018, 
//     0.02, 0.022, 0.024, 0.026, 0.028, 
//     0.03, 0.035, 0.038, 0.04, 0.042, 
//     0.044, 0.05,0.06}; // m
//     double Soil_Moisture = 0.2;
//     double Soil_Porosity = 0.482;
//     double Soil_Conduct_Sat = 0.00078;
//     double Air_Entry_Pres = 0.405;
//     double b = 11.4;
//     int step_time = 24;
//     int i;
//     double SI;
//     for (i = 0; i < 18; i++)
//     {
//         SI = Soil_Infiltration(
//         Water_input[i] / step_time,
//         Soil_Moisture,
//         Soil_Porosity,
//         Soil_Conduct_Sat,
//         Air_Entry_Pres,
//         b,
//         step_time);
//         printf("%f %f\n", Water_input[i] * 1000, SI * 1000);
//     }
//     return 0;
// }
