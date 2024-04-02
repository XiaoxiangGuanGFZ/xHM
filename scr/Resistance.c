/*
 * SUMMARY:      Resistance.c
 * USAGE:        Calculate aerodynamic and canopy resistance
 *                  used in evapotranspiration simulation
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Nov-2023
 * DESCRIPTION:  Calculate aerodynamic and canopy resistance
 * DESCRIP-END.
 * FUNCTIONS:
 *               Resist_aero_o(); Resist_aero_u()
 * COMMENTS:
 * REFERENCEs:
 *
 * Storck, P., Trees, snow and flooding:
 *      an investigation of forest canopy effects on snow accumulation
 *      and melt at the plot and watershed scales in the Pacific Northwest,
 *      Water Resource Series, Technical Report 161,
 *      Dept. of Civil Engineering, University of Washington, 2000.
 *
 * Dickinson, R. E., A. Henderson-Sellers, C. Rosenzweig, and P. J. Sellers,
 *      Evapotranspiration models with canopy resistance for
 *      use in climate models, a review, Agric. For. Meteorol., 54, 373-388, 1991.
 *
 * Dickinson, R. E., A. Henderson-Sellers, and P. J. Kennedy,
 *      Biosphere-atmosphere transfer scheme (BATS) Version leas
 *      coupled to the NCAR Community Climate Model, NCAR Technical Note,
 *      NCARITN-387+STR, Boulder, Colorado, 1993.
 *
 * Feddes, R. A., P. J. Kowalik, and H. Zaradny,
 *      Simulation of field water use and crop yield,
 *      John Wiley and Sons, New York, 188 pp., 1978.
 */

#include <stdio.h>
#include <math.h>
#include "Constants.h"
#include "Resistance.h"
#include "Func_Tem.h"

/*********** aerodynamic resistance ************/

double Resist_aero_o(
    double Air_ws_obs, /* wind speed at the measurement height, [m/s] */
    double ws_obs_z,   /* the measurement height, m */
    double zr,         /* (above canopy) reference height, a value greater than Canopy_h, [m] */
    double Canopy_h,   /* height of canopy, m */
    double d,          /* displacement height of canopy, m */
    double z0,         /* the roughness height of canopy, m */
    double dg,         /* displacement height of ground/surface, m */
    double z0_g        /* the roughness height of ground/surface, m */
)
{
    /*********
     * calculate the aerodynamic resistance above overstory
     *
     * The vertical wind profile through the overstory canopy is
     *      modeled assuming neutral atmospheric conditions
     *      using three layers (Storck, 2000).
     *
     */
    double Rao;       // aerodynamic resistance, h/m
    double k = 0.4;   // Von Karmen's constant, a dimensionless constant
    double Air_ws_zr; // wind speed at (above canopy) reference height, a value greater than Canopy_h
    double na = 2.5;  // a dimensionless extinction coefficient
    double zw;        // height of the boundary between the upper logarithmic profile and roughness sublayer

    /*****
     * compute the wind speed at above canopy reference height,
     *      by using the observed wind speed at the ground reference height, 
     *      assuming a logarithm wind profile (between anemometer and ground surface, there is no canopy).
     * Wind speed at the above canopy reference height (it is high enough) is 
     *      the same throughout the calculating grid cell (area). 
     * Wind speed is measured usually at the height of 10m above ground,
     *      and the ground surface usually is either grassland or snowpack.
    */
    Air_ws_zr = WindSpeed_Profile(
        ws_obs_z, zr, Air_ws_obs,
        dg, z0_g);
    if (Air_ws_zr < 0.0001)
    {
        Air_ws_zr = 0.0001;
    }
    // Rao = pow(log((zr - d) / z0), 2) / (Air_ws_zr * k * k);
    zw = 1.5 * Canopy_h - 0.5 * d;
    Rao = log((zr - d) / z0) / (Air_ws_zr * k * k) *
          (Canopy_h / (na * (zw - d)) *
               (exp(na * (1 - (d - z0) / Canopy_h) - 1)) +
           (zw - Canopy_h) / (zw - d) +
           log((zr - d) / (zw - d))); // unit: s/m
    // 1 h = 60 * 60 s
    // printf("Air_ws_obs: %f, d: %f, z0: %f, Rau: %f\n",Air_ws_obs, d, z0, Rao);
    return Rao / 3600; // unit: h/m
}

double Resist_aero_u(
    double Air_ws_obs, /* wind speed at the measurement height, m/s */
    double ws_obs_z,   /* the measurement height, m */
    double d,          /* displacement height, m */
    double z0          /* roughness length, m */
)
{
    /****
     * calculate aerodynamic resistance [h/m] for
     * the soil surface, snow, or understory
     * refer to Storck, 2000
     * */
    
    double za;
    double Air_ws_za;
    double Rau;     // aerodynamic resistance, h/m
    double k = 0.4; // Von Karmen's constant, a dimensionless constant

    za = 2 + d + z0;
    Air_ws_za = WindSpeed_Profile(
        ws_obs_z, za, Air_ws_obs,
        d, z0);

    if (Air_ws_za < 0.0001)
    {
        /*****
         * when Air_ws_za equals 0.0, then the 
         * aerodynamic resistance formula is invalid. 
         * we just assume the minimum valid wind speed is 0.1 m/s
        */
        Air_ws_za = 0.0001;   
    }
    Rau = pow(log((za - d) / z0), 2) / (Air_ws_za * k * k); // unit: s/m
    //    printf("Air_ws_obs: %f, d: %f, z0: %f, Rau: %f\n",Air_ws_obs, d, z0, Rau);
    return Rau / 3600;                                      // unit: h/m
}

double WindSpeed_Profile(
    double z1,  /* height 1, m */
    double z2,  /* height 2, m */
    double ws1, /* wind speed at the height 1, m/s */
    double d,   /* displacement height, m */
    double z0   /* roughness lenght, m */
)
{
    /* the wind profile above the canopy is assumed to
     * follow a logarithmic profile
     */
    double ws2;
    ws2 = ws1 * log((z2 - d) / z0) / log((z1 - d) / z0);
    return ws2;
}

/*********** canopy resistance ************/
double Resist_Canopy(
    double rs[],  /* vector: stomatal resistance, h/m */
    double LAI[], /* vector: LAI for individual leaves (species) */
    int n         /* scalar: number of species */
)
{
    /*****
     * the canopy resistance is represented as
     *  a summation of the stomatal resistance
     *  of individual leaves
     *
     * Wigmosta et al. 1994; Dickinson et al., 1991
     */
    int i;
    double rc; // canopy resistance, h/m
    double integral = 0.0;
    double LAI_c = 0.0;
    for (i = 0; i < n; i++)
    {
        LAI_c += LAI[i];
        integral += LAI[i] / rs[i];
    }
    rc = 1 / integral;

    return rc;
}

double Resist_Stomatal(
    double Air_tem_avg, /* air temperature (in degrees Celsius) */
    double Air_tem_min, /* scalar: minimum air temperature (℃)*/
    double Air_tem_max, /* scalar: maximum air temperature (℃)*/
    double Air_rhu,     /* relative humidity, % */

    double Rp,     /* the visible radiation */
    double Rpc,    /* the light level where rs is twice the rs_min */
    double rs_min, /* minimum stomatal resistance, [s/m] */
    double rs_max, /* maximum (cuticular) resistance, [s/m] */

    double SM,     /* average soil moisture content */
    double SM_wp,  /* the plant wilting point */
    double SM_free /* the moisture content above which
                   soil conditions do not
                   restrict transpiration. */
)
{
    /******
     * the stomatal resistance (of individual leaves, [h/m]) depends on
     *      vegetation type and environmental factors (
     *      air temperature, vapor pressure deficit,
     *      photosynthetically active radiation flux, and soil moisture
     *      )
     */
    double f1, f2, f3, f4;
    f1 = Factor_1(Air_tem_avg);
    f2 = Factor_2(Air_tem_min, Air_tem_max, Air_rhu);
    f3 = Factor_3(Rp, Rpc, rs_min, rs_max);
    f4 = Factor_4(SM, SM_wp, SM_free);

    double rs;
    rs = rs_min * f1 * f2 * f3 * f4;
    rs = rs / 3600; // convert unit from s/m to h/m
    return rs;
}

double Factor_1(
    double Air_tem_avg /* air temperature (in degrees Celsius) */
)
{
    /****
     * the effect of air temeprature on
     *  stomatal resistance
     * Dickinson et al. (1993)
     */
    double f1;
    f1 = 1 / (0.08 * Air_tem_avg - 0.0016 * pow(Air_tem_avg, 2));
    return f1;
}

double Factor_2(
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_rhu      /* relative humidity, % */ 
)
{
    /****
     * the effect of vapor pressure on
     *  stomatal resistance
     * Dickinson et al. (1993)
     */
    double es, ea;
    double ec = 4; // the vapor pressure deficit causing stomatal closure (about 4 kPa)
    double f2;
    es = 0.5 * (e0(Air_tem_max) + e0(Air_tem_min));
    /***
     * the function e0() is defined in Evapotranspiration.c
     */
    ea = Air_rhu * es / 100;
    f2 = 1 / (1 - (es - ea) / ec);
    return f2;
}

double Factor_3(
    double Rp,     /* the visible radiation, [W/m2] */
    double Rpc,    /* the light level where rs is twice the rs_min, [W/m2] */
    double rs_min, /* minimum stomatal resistance, [s/m] */
    double rs_max  /* maximum (cuticular) resistance, [s/m] */
)
{
    /****
     * calculate the effect of photosynthetically active radiation flux (PAR)
     *  on stomatal resistance
     *
     * Dickinson et al. (1993)
     */
    double f3;
    /****
     * rs_min and rs_max:
     * from Table 1 in Wigmosta, 1994
     *
     * Rp:
     * average PAR on a leaf surface,
     * empirically assumed as about one-tenth (0.0) the intensity
     * of the incident solar radiation [Dickinson, 1991]
     *
     * Rpc:
     * 30 W/m2 for four different vegetation:
     * Grass, Aspen, Fir, and Pine
     * [Dickinson, 1991; Wigmosta, 1994]
     *
     */
    f3 = (1 + Rp / Rpc) / (rs_min / rs_max + Rp / Rpc);
    return f3;
}

double Factor_4(
    double SM,     /* average soil moisture content */
    double SM_wp,  /* the plant wilting point */
    double SM_free /* the moisture content above which
                        soil conditions do not
                        restrict transpiration, field capacity. */
)
{
    /****
     * calculate the effect of soil moisture
     *  on stomatal resistance
     *
     *  Feddes et al. (1978)
     */
    double f4;
    if (SM <= SM_wp)
    {
        f4 = 0.0;
    }
    else if (SM <= SM_free)
    {
        f4 = (SM_free - SM_wp) / (SM - SM_wp);
    }
    else
    {
        f4 = 1.0;
    }
    return f4;
}


// double PotentialEvaporation(
//     double Air_tem_avg, /*scalar: average air tempeature (℃)*/
//     double Air_tem_min, /*scalar: minimum air temperature (℃)*/
//     double Air_tem_max, /*scalar: maximum air temperature (℃)*/
//     double Air_pres,    /* air pressure, kPa */ 
//     double Air_rhu,     /* relative humidity, % */ 
//     double Radia_net,   /* the net radiation flux density, kJ/h/m2 */ 
//     double Resist_aero
//     /***
//      * Resist_aero: the aerodynamic resistance to vapor transport
//      * between the overstory and the reference height, with the unit of h/m
//      */
// )
// {
//     /******
//      * calculate the potential evaporation rate [m/h], following (Wigmosta et al., 1994)
//      * this is the maximum water that can be evaporated and absorbed by the atmosphre,
//      * under the weather conditions. The estimated real evaporation and transpiration 
//      * are restricted under the potential evaporation.
//      * 
//     */
//     double delta;  // the slope of saturation vapor pressure curve (kPa/℃)
//     double es, ea; // saturated, and actual vapor pressure (kPa)
    
//     double gamma;  // psychrometric constant, kPa/℃
//     double Ep;

//     es = 0.5 * (e0(Air_tem_max) + e0(Air_tem_min));
//     ea = Air_rhu * es / 100;
    
//     delta = VaporPresSlope(
//         Air_tem_avg, Air_tem_min, Air_tem_max);
//     gamma = Const_psychrometric(Air_pres);
    
//     Ep = (delta * Radia_net +
//           Density_air * SpecificHeat_air * (es - ea) / Resist_aero) /
//          (lambda_v * (delta + gamma)); // unit: kg/m2/h
//     Ep = Ep / 1000;  // m/h
//     return Ep;
// }

// double PotentialEvaporation(
//     double Air_tem_avg, /*scalar: average air tempeature (℃)*/
//     double Air_tem_min, /*scalar: minimum air temperature (℃)*/
//     double Air_tem_max, /*scalar: maximum air temperature (℃)*/
//     double Air_pres,    /* air pressure, kPa */ 
//     double Air_rhu,     /* relative humidity, % */ 
//     double Radia_net,   /* the net radiation flux density, kJ/h/m2 */ 
//     double Resist_aero
//     /***
//      * Resist_aero: the aerodynamic resistance to vapor transport
//      * between the overstory and the reference height, with the unit of h/m
//      */
// );
// int main(int argc, char const *argv[])
// {
//     double ws_obs_z = 10;
//     double Air_ws_obs[18] = {0, 0.1, 0.2, 0.3, 0.5, 0.8, 1.0, 1.2, 1.5, 1.8,2.0, 2.5, 2.8, 3.0, 3.2, 3.4, 3.8, 4.0};
//     double zr = 50;
//     double Canopy_h = 14;
//     double d = 6.7;
//     double z0 = 1.23;
//     double dg = 0.2;
//     double z0_g = 0.05;

//     double Resist_aero;
//     double Resist_aero_under;
//     double C_aero;
//     double Ep;
//     double Ep_u;

//     double Air_pres = 98;
//     double Air_rhu = 85;
//     double Air_tem_avg = 10;
//     double Air_tem_max = 15;
//     double Air_tem_min = 5.7;
//     double Radia_net = 245;
//     for (size_t i = 0; i < 18; i++)
//     {
//         Resist_aero = Resist_aero_o(
//             Air_ws_obs[i], /* wind speed at the measurement height, [m/s] */
//             ws_obs_z,      /* the measurement height, m */
//             zr,            /* (above canopy) reference height, a value greater than Canopy_h, [m] */
//             Canopy_h,      /* height of canopy, m */
//             d,             /* displacement height of canopy, m */
//             z0,            /* the roughness height of canopy, m */
//             dg,            /* displacement height of ground/surface, m */
//             z0_g           /* the roughness height of ground/surface, m */
//         );

//         Resist_aero_under = Resist_aero_u(
//             Air_ws_obs[i], /* wind speed at the measurement height, m/s */
//             ws_obs_z,      /* the measurement height, m */
//             d,             /* displacement height, m */
//             z0             /* roughness length, m */
//         );

//         Ep = PotentialEvaporation(
//             Air_tem_avg, /*scalar: average air tempeature (℃)*/
//             Air_tem_min, /*scalar: minimum air temperature (℃)*/
//             Air_tem_max, /*scalar: maximum air temperature (℃)*/
//             Air_pres,    /* air pressure, kPa */
//             Air_rhu,     /* relative humidity, % */
//             Radia_net,   /* the net radiation flux density, kJ/h/m2 */
//             Resist_aero);
//         Ep_u = PotentialEvaporation(
//             Air_tem_avg, /*scalar: average air tempeature (℃)*/
//             Air_tem_min, /*scalar: minimum air temperature (℃)*/
//             Air_tem_max, /*scalar: maximum air temperature (℃)*/
//             Air_pres,    /* air pressure, kPa */
//             Air_rhu,     /* relative humidity, % */
//             Radia_net,   /* the net radiation flux density, kJ/h/m2 */
//             Resist_aero_under);
//         C_aero = 1 / Resist_aero;
//         printf("%.1f %10.4f %10.4f %6.2f %6.2f\n", Air_ws_obs[i], C_aero, 1/Resist_aero_under, Ep * 24 * 1000, Ep_u *24*1000);
//     }
    
//     return 0;
// }
