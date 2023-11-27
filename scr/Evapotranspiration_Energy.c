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
 * double Ref_u         - overstory reflection coefficient
 * double L_sky         - downward sky longwave radiation flux
 * double Lo            - overstory longwave radiation flux
 * double Lu            - upward understory longwave radiation flux
 * double Tau           - fraction of shortwave radiation transmitted by canopy
 * double Tem_o, Tem_u  - temperature of overstory and understory, [Celsius degress]
 * double Tem_s         - soil surface temperature, [Celsius degress]
 *                             only valid when no understory and no snowpack
 * 
 * double *Rno          - net radiation for overstory
 * double *Rnu          - net radiation for understory
 * double LAIo          - type of the cell:
 *                        0: no understory and no snow (bare soil)
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
    double *Rnu,
    double Frac_canopy,
    double Ref_o,
    double Ref_u,
    double Tem_o,
    double Tem_u,
    double Tem_s,
    double LAIo,
    int Vtype
)
{
    double Tau;
    Tau = exp( - LAIo);
    double Lu, Lo;
    Lo = Emissivity(Tem_o);
    if (Vtype == 0) {
        Lu = Emissivity(Tem_s); 
    } else {
        Lu = Emissivity(Tem_u);
    }    

    *Rno = Rs * ((1-Ref_o) - Tau * (1 - Ref_u)) * Frac_canopy + 
        (L_sky + Lu - 2*Lo) * Frac_canopy;
    *Rnu = Rs * (1-Ref_u) * (1-Frac_canopy + Tau * Frac_canopy) + 
        (1 - Frac_canopy) * L_sky + Frac_canopy * Lo - Lu;

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