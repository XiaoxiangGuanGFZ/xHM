/*
 * SUMMARY:      Evapotranspiration.c
 * USAGE:        Calculate evapotranspiration (two layers model)
 *               overstory canopy and understory 
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Nov-2023
 * DESCRIPTION:  Calculate evapotranspiration
 * DESCRIP-END.
 * FUNCTIONS:    PotentialEvaporation(), Transpiration(), VaporPresSlope()
 *                  Const_psychrometric(), e0(), Kelvin_tem(), WET_part(),
 *                  ET_iteration()
 * 
 * COMMENTS:
 * 
 *
 * REFERENCES:
 * Wigmosta, M.S., L. W. Vail, and D. P. Lettenmaier,
 *      A distributed hydrologyvegetation model
 *      for complex terrain, Water Resources Research, 30 (6), 1665-1679, 1994.
 *
 * Dickinson, R. E., A. Henderson-Sellers, and P. J. Kennedy,
 *      Biosphere-atmosphere transfer scheme (BATS) Version leas coupled to the
 *      NCAR Community Climate Model, NCAR Technical Note, NCARITN-387+STR,
 *      Boulder, Colorado, 1993.
 *
 */

#include <stdio.h>
#include <math.h>
#include "Constants.h"
#include "Evapotranspiration.h"
#include "Evapotranspiration_ST.h"
#include "Evapotranspiration_Energy.h"
#include "Evaporation_soil.h"
#include "Radiation_Calc.h"
#include "Resistance.h"


double PotentialEvaporation(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_pres,    /* air pressure, kPa */ 
    double Air_rhu,     /* relative humidity, % */ 
    double Radia_net,   /* the net radiation flux density, kJ/h/m2 */ 
    double Resist_aero
    /***
     * Resist_aero: the aerodynamic resistance to vapor transport
     * between the overstory and the reference height, with the unit of h/m
     */
)
{
    /******
     * calculate the potential evaporation rate [m/h], following (Wigmosta et al., 1994)
     * this is the maximum water that can be evaporated and absorbed by the atmosphre,
     * under the weather conditions. The estimated real evaporation and transpiration 
     * are restricted under the potential evaporation.
     * 
    */
    double delta;  // the slope of saturation vapor pressure curve (kPa/℃)
    double es, ea; // saturated, and actual vapor pressure (kPa)
    
    double gamma;  // psychrometric constant, kPa/℃
    double Ep;

    es = 0.5 * (e0(Air_tem_max) + e0(Air_tem_min));
    ea = Air_rhu * es / 100;
    
    delta = VaporPresSlope(
        Air_tem_avg, Air_tem_min, Air_tem_max);
    gamma = Const_psychrometric(Air_pres);
    
    Ep = (delta * Radia_net +
          Density_air * SpecificHeat_air * (es - ea) / Resist_aero) /
         (lambda_v * (delta + gamma)); // unit: kg/m2/h
    Ep = Ep / 1000;  // m/h
    return Ep;
}

double Transpiration(
    double Evapoation_pot, /* appropriate potential evaporation, [m/h] */ 
    double Air_tem_avg,    /* scalar: average air tempeature (℃)*/
    double Air_tem_min,    /* scalar: minimum air temperature (℃)*/
    double Air_tem_max,    /* scalar: maximum air temperature (℃)*/
    double Air_pres,       /* air pressure, [kPa] */ 
    double Resist_canopy,  /* aerodynamic resistance to vapor transport, [h/m] */ 
    double Resist_aero     /* canopy resistance to vapor transport, [h/m] */
){
    /*******
     * Transpiration from dry vegetative surfaces is 
     *      calculated using a 
     *      Penman-Monteith approach (Wigmosta et al., 1994)
     * 
    */
    double Transpiration; 
    /****
     * transpiration rate at vegetative surfaces, 
     * share the same unit with potentoal evaporation rate, [m/h]
     * */ 

    double delta;  // the slope of saturation vapor pressure curve (kPa/℃)
    double gamma;  // psychrometric constant, kPa/℃
    delta = VaporPresSlope(
        Air_tem_avg, Air_tem_min, Air_tem_max);
    gamma = Const_psychrometric(Air_pres);

    Transpiration = Evapoation_pot *
                    (delta + gamma) / (delta + gamma * (1 + Resist_canopy / Resist_aero));

    return Transpiration;
}


double VaporPresSlope(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max  /*scalar: maximum air temperature (℃)*/
){
    /*****
     * calculate the slope of saturation vapor pressure curve 
     * based on air temperature, [℃]
     * 
    */
    double delta; // the slope of saturation vapor pressure curve (kPa/℃)

    delta = 4098 * e0(Air_tem_avg) / pow(0.5*(Air_tem_min + Air_tem_max) + 237.3, 2.0);  
    return delta;
}

double Const_psychrometric(
    double Air_pres  /* air pressure, [kPa] */ 
){
    /***
     * [psychrometric constant] (https://en.wikipedia.org/wiki/Psychrometric_constant) 
     *  depends upon altitude, as air pressure, 
     *  On average, at a given location or altitude, 
     *  the psychrometric constant is approximately constant.
     *  standard air pressure: 101.3 kPa, if air pressure data is missing
    */
    double gamma; // psychrometric constant, kPa/℃
    gamma = 0.665 * pow(10, -3) * Air_pres ; 
    return gamma;
}


double e0(
    double Air_tem  /* scalar: air temperature (celsius degree) */
) {
    /****
     * estimate saturated water-vapor pressures (kPa) from air temperature
     * 
    */
    double e0;
    e0 = 0.6108 * exp(17.277 * Air_tem / (Air_tem + 273.3));
    return e0;
}

double Kelvin_tem(
    double tem
) {
    /* convert the temperature from celsius degree to Kelvin K */
    return tem + 273.15;
}

double WET_part(
    double Prec,         /* scalar: precipitation, [m] */
    double Interception, /* scalar: the depth of intercepted water at the start of the time step, [m] */
    double LAI,
    double Frac_canopy   /* scalar: the fraction of ground surface covered by the canopy */
){
    /*******
     * every story (over and under) is partitioned into 
     * a wet fraction (Aw) and a dry fraction (1 - Aw),
     * following Dickinson et al (1993)
     * 
    */
    double Ic;  // the maximum interception storage capacity (in meters)
    double Aw;  // wet fraction, value range: [0, 1] 
    Ic = 0.0001 * LAI * Frac_canopy;
    Aw = pow((Interception + Prec)/Ic, 2/3);
    if (Aw > 1) {
        Aw = 1.0;
    } else if (Aw < 0.0)
    {
        Aw = 0.0;
    }
    return Aw;
}


/***** evapotranspiration (ET) ********/
void ET_story(
    double Air_tem_avg,       /*scalar: average air tempeature (℃)*/
    double Air_tem_min,       /*scalar: minimum air temperature (℃)*/
    double Air_tem_max,       /*scalar: maximum air temperature (℃)*/
    double Air_pres,          /* air pressure, kPa */ 

    double Prec_input,        /* precipitation input to the story, m */
    double *Prec_throughfall, /* free water leaving the story, m */
    double Ep,                /* potential evaporation, m/h */
    double *EI,               /* actual evaporation, m */
    double *ET,               /* actual transpiration, m */
    double *Interception,     /* intercepted water in the story, m */
    double Resist_canopy,     /* canopy resistance, h/m */
    double Resist_aero,       /* aerodynamic resistance, h/m */
    double LAI,
    double Frac_canopy,       /* canopy fraction */
    int step_time             /* time interval, time step; hours*/ 

){
    /***********************
     * evapotranspiration calculation for each story (overstory or understory)
     * 
     * for overstory:
     * - Ep: estimated from function PotentialEvaporation()
     * - Prec_input: precipitation
     * - Frac_canopy: canopy fraction, (0, 1]
     * - *Prec_throughfall: throughfall from overstory to understory
     * 
     * for understory:
     * - Ep: modified Ep after overstory
     * - Prec_input: throughfall
     * - Frac_canopy: 1
     * - *Prec_throughfall: excess precipitation into soil process (either infiltration or runoff)
    */
    double Ic;
    // double Ep;
    double Et;
    double Aw;
    double tw;

    Ic = 0.0001 * LAI * Frac_canopy;
    if (Ep > 0.0)
    {
        Et = Transpiration(
            Ep, Air_tem_avg, Air_tem_min, Air_tem_max,
            Air_pres, Resist_canopy, Resist_aero);

        Aw = WET_part(
            Prec_input, *Interception, LAI, Frac_canopy);

        tw = (*Interception + Prec_input) / (Ep * Aw);

        if (tw <= (double)step_time)
        {
            /***
             * the overstory intercepted and precipitation
             *  are evaporated within the time step
             */
            *EI = (Ep * Aw) * tw;                     // evaporation from wet part, [m]
            *ET = Et * (1 - Aw) * tw +                // transpiration from wet part, [m]
                  Et * Aw * ((double)step_time - tw); //  from dry part
            
        }
        else
        {
            /****
             * the intercepted water and precipitation are enough for
             * evaporation within the time step
             */
            *EI = Ep * step_time;
            *ET = 0.0; // no transpiration
        }
        if (*EI < 0.0)
        {
            *EI = 0.0;
        }
        if (*ET < 0.0)
        {
            *ET = 0.0;
        }
        
    }
    else
    {
        /* Ep <= 0.0 */
        *EI = 0.0;
        *ET = 0.0;
    }

    /*** update the story state ***/
    double Prec_excess;
    Prec_excess = (*Interception + Prec_input) - *EI;
    if (Prec_excess <= Ic)
    {
        *Interception = Prec_excess; // update the overstory interception water
        *Prec_throughfall = 0.0;
    }
    else
    {
        *Interception = Ic;
        *Prec_throughfall = Prec_excess - Ic;
    }
}

void ET_iteration(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_pres,    /* air pressure, kPa */ 
    double Air_rhu,     /* relative humidity, % */ 
    double Radia_net,   /* the net radiation flux density on the cell surface, kJ/h/m2 */ 
    
    double Prec,                /* precipitation (total) within the time step, m */
    double *Prec_throughfall,   /* precipitation throughfall from overstory, [m] */
    double *Prec_net,           /* net precipitation from understory into soil process, [m] */
    double *Ep,                 /* the potential evaporation rate, [m/h] */
    double *EI_o,               /* actual evaporation from overstory, m */
    double *ET_o,               /* actual transpiration from overstory, m */
    double *EI_u,               /* actual evaporation from understory, m */
    double *ET_u,               /* actual transpiration from understory, m */
    double *ET_s,               /* soil evaporation, m */
    double *Interception_o,     /* overstory interception water, m */
    double *Interception_u,     /* understory interception water, m */
    double Resist_canopy_o,
    double Resist_canopy_u,
    double Resist_aero_o,
    double Resist_aero_u,
    /***
     * Resist_aero_o: the aerodynamic resistance to vapor transport
     * between the overstory and the reference height, with the unit of h/m
     */

    double LAI_o,
    double LAI_u,
    double Frac_canopy,    /* canopy fraction, [0, 1] */  
    double Soil_Fe,        /* soil desorptivity, [m] */  
    int Toggle_Understory, /* 1: there is an understory */ 
    int step_time          /* iteration time step: in hours */ 
 )
 {
    /****************** description ***************
     * simulate the two-layer evapotranspiration process in a stepwise manner
     * the double type pointers are iterated, indicating the state variables 
     *  
     * Scenarios: 
     * when the overstory is absent, 
     * Resist_canopy_o and Resist_aero_o can be ignored
     * 
     * when there is an understory,
     * soil evaporation *ET_s is ignored, or equals 0.0
     * 
     * Toggle_Overstory != 1 and Toggle_Understory != 1, to 
     * simulate the bare soil (the only story) ET .
     * 
    */
    /******* overstory *******/
    int Toggle_Overstory = 1;
    if (Frac_canopy < 0.0001)
    {
        Toggle_Overstory = 0;
        Frac_canopy = 0.0;
    }
    
    double Ep_u;
    if (Toggle_Overstory == 1)
    {
        *Ep = PotentialEvaporation(
            Air_tem_avg, Air_tem_min, Air_tem_max,
            Air_pres, Air_rhu, Radia_net, Resist_aero_o);
        ET_story(Air_tem_avg, Air_tem_min, Air_tem_max, Air_pres,
                 Prec, Prec_throughfall,
                 *Ep,
                 EI_o, ET_o,
                 Interception_o, Resist_canopy_o, Resist_aero_o, LAI_o,
                 Frac_canopy, step_time // hours
        );
    }
    else
    {
        // there is no overstory, but understory
        *ET_o = 0.0;
        *EI_o = 0.0;
        *Prec_throughfall = Prec;
        *Interception_o = 0.0;
        *Ep = PotentialEvaporation(
            Air_tem_avg, Air_tem_min, Air_tem_max,
            Air_pres, Air_rhu, Radia_net, Resist_aero_u);
    }    
    
    /************ understory *************/
    Ep_u = *Ep - (*ET_o + *EI_o) / step_time;
    if (Toggle_Understory == 1)
    {
        ET_story(Air_tem_avg, Air_tem_min, Air_tem_max, Air_pres,
                 *Prec_throughfall, Prec_net,
                 Ep_u,
                 EI_u, ET_u,
                 Interception_u, Resist_canopy_u, Resist_aero_u, LAI_u,
                 1.0,      // Frac_canopy = 1.0
                 step_time // hours
        );
        *ET_s = 0.0;
    }
    else
    {
        /* there is only bare soil for this cell 
        * no overstory, no understory
        */
        *ET_s = ET_soil(Ep_u, Soil_Fe / step_time) * step_time;
        *EI_u = 0.0;
        *ET_u = 0.0;
        *Interception_u = 0.0;
        *Prec_net = *Prec_throughfall - *ET_s;
        if (*Prec_net < 0.0)
        {
            *Prec_net = 0.0;
        }
    }
    // printf("Ep: %8.2f\n", *Ep * step_time * 1000); // unit:mm 
    // printf("%s:%8.2f %s:%8.2f %s:%8.2f %s:%8.2f %s:%8.2f\n",
    // "EI_o", *EI_o * 1000, "ET_o", *ET_o * 1000, "EI_u", *EI_u * 1000, "ET_u", *ET_u * 1000, "ET_s", *ET_s * 1000
    // );
}


void ET_CELL(
    int year,
    int month,
    int day,
    double lat,         /* the latitute of the location */

    double Prec,        /* precipitation (total) within the time step, m */
    double Air_tem_avg, /* air temperature (in degrees Celsius) */
    double Air_tem_min, /* scalar: minimum air temperature (℃)*/
    double Air_tem_max, /* scalar: maximum air temperature (℃)*/
    double Air_rhu,     /* relative humidity, unit: % */
    double Air_pres,    /* air pressure, kPa */
    double Air_ws_obs,  /* wind speed at the measurement height, m/s */
    double ws_obs_z,    /* the measurement height, m */
    double Air_ssd,     /* sunshine duration in a day, hours */

    double *Rs,         /* received shortwave radiation for the overstory canopy, [kJ/m2/h] */
    double *L_sky,      /* received longwave radiation for the overstory canopy, [kJ/m2/h] */
    double *Rno,        /* net radiation for the overstory, [kJ/m2/h] */
    double *Rno_short,  /* net shortwave radiation for the overstory, [kJ/m2/h] */
    double *Rnu,        /* net radiation for the understory, [kJ/m2/h] */
    double *Rnu_short,  /* net shortwave radiation for the understory, [kJ/m2/h] */
    double *Rns,        /* net radiation for ground/soil, [kJ/m2/h] */

    double Frac_canopy, /* the fractional forest cover, between 0.0 and 1.0 */
    double Ref_o,       /* reflection coefficient of radiation for overstory */
    double Ref_u,       /* reflection coefficient of radiation for understory */
    double Ref_s,       /* reflection coefficient of radiation for soil/ground */
    double LAI_o,       /* LAI for overstory */
    double LAI_u,       /* LAI for understory */
    double Rpc_o,       /* the light level where rs is twice the rs_min, [W/m2] */
    double rs_min_o,    /* minimum stomatal resistance, [s/m] */
    double rs_max_o,    /* maximum (cuticular) resistance, [s/m] */
    double Rpc_u,       /* the light level where rs is twice the rs_min */
    double rs_min_u,    /* minimum stomatal resistance, [s/m] */
    double rs_max_u,    /* maximum (cuticular) resistance, [s/m] */
    double Canopy_zr,   /* reference height of canopy, m */
    double Canopy_h,    /* height of canopy, m */
    double d_o,         /* displacement height of canopy, m */
    double z0_o,        /* the roughness of canopy, m */
    double d_u,         /* displacement height of understory, m */
    double z0_u,        /* roughness length of understory, m */
    
    double SM,          /* average soil moisture content */
    double SM_wp,       /* the plant wilting point */
    double SM_free,     /* the moisture content above which soil conditions do not restrict transpiration, field capacity. */
    double Soil_Fe,     /* soil desorptivity, [m] */

    double *Prec_throughfall, /* precipitation throughfall from overstory, [m] */
    double *Prec_net,         /* net precipitation from understory into soil process, [m] */
    double *Ep,               /* the potential evaporation rate, [m/h] */
    double *EI_o,             /* actual evaporation, m */
    double *ET_o,             /* actual transpiration, m */
    double *EI_u,             /* actual evaporation, m */
    double *ET_u,             /* actual transpiration, m */
    double *ET_s,             /* soil evaporation, m */
    double *Interception_o,   /* overstory interception water, m */
    double *Interception_u,   /* understory interception water, m */
    int Toggle_Understory,    
    int step_time             /* iteration time step: in hours */
)
{
    /******************************
     * compute the radiation received over the two layers
     * From sky, considering the effect of cloud cover, 
     *      only controlled by weather conditions and geolocation
     * - shortwave radiation from sky
     * - longwave radiation from sky
     * 
     * For two layers: overstory and understory
     * - net radiation for two layers
     * - net shortwave radiation for two layers
    */

    /***** 
     * unit for the sky shortwave and longwave radiations: [MJ/m2/d] 
     * [MJ/m2/d] = 1000 000 / (3600*24) W/m2, as J/s = W 
     * 1000 000 / (3600*24) = 11.574
    */
    *Rs = Radiation_downward_short(year, month, day, lat, Air_ssd);
    *L_sky = Radiation_downward_long(
        year, month, day, lat,
        Air_tem_avg, Air_rhu, Air_ssd, 0.0);

    /*****
     * convert the radiation unit:
     * from [MJ/m2/d] to [kJ/m2/h] 
    */
    *Rs = *Rs * 1000/24;
    *L_sky = *L_sky * 1000/24;
    // printf("Rs: %8.2f\nL_sky: %8.2f\n", *Rs, *L_sky);
    int Toggle_Overstory = 1;           /* whether there is overstory, yes: 1 */
    if (Frac_canopy < 0.0001)
    {
        Toggle_Overstory = 0;
        Frac_canopy = 0.0;
    }
    
    double Tem_o;
    double Tem_u;
    double Tem_s;
    Tem_o = Air_tem_avg;
    Tem_s = Air_tem_avg;
    Tem_u = Air_tem_avg;

    Radiation_net(
        *Rs, *L_sky,
        Rno, Rno_short, Rnu, Rnu_short, Rns,
        Frac_canopy, Ref_o, Ref_u, Ref_s,
        Tem_o, Tem_u, Tem_s,
        LAI_o, LAI_u, Toggle_Understory);
    // printf(
    //     "Rno: %8.2f\nRno_short: %8.2f\nRnu: %8.2f\nRnu_short: %8.2f\n",
    //     *Rno, *Rno_short, *Rnu, *Rnu_short
    // );
    double R_net;
    if (Toggle_Overstory == 1) 
    {
        R_net = *Rno;
    }
    else if (Toggle_Understory == 1)
    {
        R_net = *Rnu;
    }
    else
    {
        R_net = *Rns;
    }
    // printf("R_net: %8.2f\n", R_net);
    double Rp_o, Rp_u;   // visiable radiation in net shortwave radiation
    Rp_o = VISFRACT * *Rno_short * 1000/3600;  // convert kJ/m2/h to W/m2, the same unit as Rpc
    Rp_u = VISFRACT * *Rnu_short * 1000/3600;
    double Res_canopy_o; // assume only one vegetation (leaf type) for each cell
    double Res_canopy_u;
    double Res_aero_o;
    double Res_aero_u;
    
    if (Toggle_Overstory == 1)
    {
        Res_canopy_o = Resist_Stomatal(
            Air_tem_avg, Air_tem_min, Air_tem_max, Air_rhu,
            Rp_o, Rpc_o, rs_min_o, rs_max_o,
            SM, SM_wp, SM_free) / LAI_o;
        Res_aero_o = Resist_aero_o(
            Air_ws_obs, ws_obs_z, Canopy_zr,
            Canopy_h, d_o, z0_o, d_u, z0_u);
    }
    else
    {
        Res_canopy_o = 1.0;
        Res_aero_o = 1.0;
    }

    if (Toggle_Understory == 1)
    {
        Res_canopy_u = Resist_Stomatal(
            Air_tem_avg, Air_tem_min, Air_tem_max, Air_rhu,
            Rp_u, Rpc_u, rs_min_u, rs_max_u,
            SM, SM_wp, SM_free) / LAI_u;
        Res_aero_u = Resist_aero_u(
            Air_ws_obs, ws_obs_z,
            d_u, z0_u);
    } 
    else
    {
        Res_canopy_u = 1.0;
        Res_aero_u = 1.0;
    }
    // printf(
    //     "Res_canopy_o: %6.2f\nRes_aero_o: %6.2f\nRes_canopy_u: %6.2f\nRes_aero_u: %6.2f\n",
    //     Res_canopy_o, Res_aero_o, Res_canopy_u, Res_aero_u
    // );
    /***********
     * iteration for evapotranspiration computation
     * - evaporation
     * - transpiration
     * 
     * Two layers:
     * - overstory (canopy)
     * - understory (lower canopy or bare soil)
     * 
    */
    ET_iteration(
        Air_tem_avg, Air_tem_min, Air_tem_max, Air_pres, Air_rhu,     
        R_net,        
        Prec, Prec_throughfall, Prec_net, 
        Ep,        
        EI_o, ET_o, EI_u, ET_u, ET_s,             
        Interception_o, Interception_u,   
        Res_canopy_o, Res_canopy_u,
        Res_aero_o, Res_aero_u,
        LAI_o, LAI_u,
        Frac_canopy,
        Soil_Fe,
        Toggle_Understory, // whether the understory is bare soil, yes: 1
        step_time);
    
}
