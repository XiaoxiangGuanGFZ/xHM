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


#include <math.h>
#include "Constants.h"
#include "Evapotranspiration.h"
#include "Evaporation_soil.h"

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
    double ET;

    es = 0.5 * (e0(Air_tem_max) + e0(Air_tem_min));
    ea = Air_rhu * es / 100;

    delta = VaporPresSlope(
        Air_tem_avg, Air_tem_min, Air_tem_max);
    gamma = Const_psychrometric(Air_pres);

    ET = (delta * Radia_net +
          Density_air * SpecificHeat_air * (es - ea) / Resist_aero) /
         (lambda_v * (delta + gamma));

    return ET;
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
}


double e0(
    double Air_tem  /* scalar: air temperature (celsius degree) */
) {
    /****
     * estimate saturated water-vapor pressures (kPa) from air temperature
     * 根据气温，计算得到瞬时饱和水汽压kPa
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
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_pres,    /* air pressure, kPa */ 

    double Prec_input,  /* precipitation input to the story*/
    double *Prec_throughfall, /* free water leaving the story*/
    double Ep,                /* potential evaporation, m/h */
    double *EI,               /* actual evaporation, m */
    double *ET,               /* actual transpiration, m */
    double *Interception,     /* intercepted water in the story */
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
    double Radia_net,   /* the net radiation flux density, kJ/h/m2 */ 
    
    double Prec,                /* precipitation (total) within the time step, m */
    double *Prec_throughfall,   /* precipitation throughfall from overstory*/
    double *Prec_net,           /* net precipitation from understory into soil process */
    double *EI_o,               /* actual evaporation, m */
    double *ET_o,               /* actual transpiration, m */
    double *EI_u,               /* actual evaporation, m */
    double *ET_u,               /* actual transpiration, m */
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
    double Frac_canopy,   //  canopy fraction, [0, 1]
    double Soil_Fe,       //  soil desorptivity
    int Toggle_Overstory,
    // whether there is overstory, yes: 1
    int Toggle_Understory_BareSoil,
    // whether the understory is bare soil, yes: 1
    int step_time         // iteration time step: in hours
 ){
    /***** description 
     * simulate the two-layer evapotranspiration process in a stepwise manner
     * the double type pointers are iterated, indicating the state variables 
     *  
     * Scenarios: 
     * when the overstory is absent, 
     * Resist_canopy_o and Resist_aero_o can be ignored
     * 
     * when the understory is not bare soil,
     * *ET_s is ignored, or equals 0.0
     * 
     * Toggle_Overstory != 1 and Toggle_Understory_BareSoil == 1, to 
     * simulate the bare soil (the only story) ET .
     * 
    */
    /************ overstory *************/
    double Ep_o, Ep_u;
    if (Toggle_Overstory == 1)
    {
        Ep_o = PotentialEvaporation(
            Air_tem_avg, Air_tem_min, Air_tem_max,
            Air_pres, Air_rhu, Radia_net, Resist_aero_o);

        ET_story(Air_tem_avg, Air_tem_min, Air_tem_max, Air_pres,
                 Prec, Prec_throughfall,
                 Ep_o,
                 EI_o, ET_o,
                 Interception_o, Resist_canopy_o, Resist_aero_o, LAI_o,
                 Frac_canopy, step_time // hours
        );
    }
    else
    {
        // there is no overstory, only understory
        Ep_o = PotentialEvaporation(
            Air_tem_avg, Air_tem_min, Air_tem_max,
            Air_pres, Air_rhu, Radia_net, Resist_aero_u);
        *ET_o = 0.0;
        *EI_o = 0.0;
        *Prec_throughfall = Prec;
        *Interception_o = 0.0;
    }
    

    /************ understory *************/
    Ep_u = Ep_o - (*ET_o + *EI_o) / step_time;
    if (Toggle_Understory_BareSoil != 1)
    {
        /* understory is not bare soil */
        
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
        /* understory is bare soil */
        *ET_s = ET_soil(
            Ep_u, Soil_Fe);
        *EI_u = 0.0;
        *ET_u = 0.0;
        *Interception_u = 0.0;
        *Prec_net = *Prec_throughfall - *ET_s;
        if (*Prec_net < 0.0)
        {
            *Prec_net = 0.0;
        }
    }
}

