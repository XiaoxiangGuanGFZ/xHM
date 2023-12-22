/*
 * SUMMARY:      Evapotranspiration_Energy.c
 * USAGE:        Calculate net radiation received by story
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Nov-2023
 * DESCRIPTION:  Calculate net radiations
 * DESCRIP-END.
 * FUNCTIONS:    
 * 
 * COMMENTS:
 * 
*/

#include <math.h>
#include "Constants.h"
#include "Evapotranspiration_Energy.h"

/*********************************************************
 * VARIABLEs:
 * double Rs            - incident shortwave radiation
 * double Frac_canopy   - fractional ground cover of the overstory
 *                          0: there is no canopy (overstory)
 *                          (0, 1]
 * double Ref_o         - overstory reflection coefficient
 * double Ref_u         - understory reflection coefficient
 * double Ref_s         - soil/ground reflection coefficient
 * double L_sky         - downward sky longwave radiation flux
 * double Lo            - overstory longwave radiation flux
 * double Lu            - upward understory longwave radiation flux
 * double Tau           - fraction of shortwave radiation transmitted by canopy
 * double Tem_o, Tem_u  - temperature of overstory and understory, [Celsius degress]
 * double Tem_s         - soil surface temperature, [Celsius degress]
 *                             only valid when no snowpack
 * 
 * double *Rno          - net radiation for overstory
 * double *Rno_short    - net shortwave radiation for overstory
 * double *Rnu          - net radiation for understory
 * double *Rnu_short    - net shortwave radiation for understory
 * double *Rns          - net radiation for ground surface (soil)
 * double LAI_o         - LAI of overstory
 * double LAI_u         - LAI of understory
 * int Toggle_Understory- type of the cell, 1: there is an understory; 0: no understory, only soil/ground
 * 
 * COMMENTs:
 * - Rs can be estimated by Radiation_short_surface() in "Radiation_Calc.c"
 * - L_sky can be calculated by Radiation_downward_long() in "Radiation_Calc.c"
 * - unit for radiation fluxes Rs, L_sky, *Rno, and *Rnu: 
 *      (MJ⋅m-2⋅d-1) = 1000 000 / (3600*24) W/m2, as J/s = W 
*/

void Radiation_net(
    double Rs,
    double L_sky,
    double *Rno,
    double *Rno_short,
    double *Rnu,
    double *Rnu_short,
    double *Rns,
    double Frac_canopy,
    double Ref_o,
    double Ref_u,
    double Ref_s,
    double Tem_o,
    double Tem_u,
    double Tem_s,
    double LAI_o,
    double LAI_u,
    int Toggle_Understory
)
{
    int Toggle_Overstory = 1;
    if (Frac_canopy < 0.0001)
    {
        Toggle_Overstory = 0;
        Frac_canopy = 0.0;
    }
    
    double Tau_o, Tau_u;
    double Lu, Lo, Ls;

    if (Toggle_Overstory == 1)
    {
        Tau_o = exp( - LAI_o);
        Lo = Emissivity(Tem_o);
    } else {
        Tau_o = 0.0;
        Lo = 0.0;
    }

    if (Toggle_Understory == 1)
    {
        Tau_u = exp( - LAI_u);
        Ls = Emissivity(Tem_s);
    } else {
        Tau_u = 1.0;
        Ls = 0.0; 
    }
    
    Lu = Emissivity(Tem_s);

    /** for overstory **/
    if (Toggle_Overstory == 1)
    {
        *Rno_short = Rs * ((1 - Ref_o) - Tau_o * (1 - Ref_u)) * Frac_canopy;
        *Rno = *Rno_short + (L_sky + Lu - 2 * Lo) * Frac_canopy;
    }
    else
    {
        *Rno_short = 0.0;
        *Rno = 0.0;
    }

    /** for understory **/
    if (Toggle_Understory == 1)
    {
        *Rnu_short = Rs * ((1 - Ref_u) - Tau_u * (1 - Ref_s)) * (1 - Frac_canopy + Tau_o * Frac_canopy);
        *Rnu = *Rnu_short +
               (1 - Frac_canopy) * (L_sky + Ls - 2 * Lu) + Frac_canopy * (Lo + Ls - 2 * Lu);
    }

    *Rns = Rs * Tau_u * (1 - Ref_s) * (1 - Frac_canopy + Tau_o * Frac_canopy) + 
        Lo * Frac_canopy + L_sky * (1 - Frac_canopy) - Ls;

}

double Emissivity(
    double Tem   /* temperature of the unity, [celsuis degree] */
)
{
    /****
     * the energy flux emitted from the unity, with a temperature of Tem
    */
    double Radiation; // unit: MJ/(m2 * d)
    double sigma = 4.903 * pow(10, -9); // Stefan-Boltzmann constant MJ/(K4*m2*d)
    Radiation = sigma * pow(Tem + 273.15, 4); //  MJ/(m2*d)
    return Radiation; 

}