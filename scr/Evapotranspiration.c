#include <math.h>
#include "Snow_GVAR.h"

double PotentialEvaporation(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_pres,  // air pressure, kPa
    double Air_rhu,   // relative humidity, %
    double Radia_net, // the net radiation flux density, kJ/h/m2
    double Resist_aero 
    /***
     * Resist_aero: the aerodynamic resistance to vapor transport 
     * between the overstory and the reference height, with the unit of h/m
    */
){
    double delta;  // the slope of saturation vapor pressure curve (kPa/℃)
    double es, ea; // saturated, and actual vapor pressure (kPa)
    
    double gamma;  // // psychrometric constant, kPa/℃
    double ET;

    es = 0.5 * (e0(Air_tem_max) + e0(Air_tem_min)); 
    ea = Air_rhu * es / 100; 

    delta = VaporPresSlope(
        Air_tem_avg, Air_tem_min, Air_tem_max
    ); 
    gamma = Const_psychrometric(Air_pres);

    ET = (
        delta * Radia_net + 
        Density_air * SpecificHeat_air * (es - ea) / Resist_aero
    ) / (lambda_v * (delta + gamma));

    return ET;
}



double Transpiration(
    double Evapoation_pot, // appropriate potential evaporation, m/h
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_pres,    // air pressure, kPa
    double Resist_canopy, // aerodynamic resistance to vapor transport, [h/m]
    double Resist_aero    // canopy resistance to vapor transport, [h/m]
){
    double Transpiration; 
    /****
     * transpiration rate at vegetative surfaces, 
     * share the same unit with Evapoation_pot, [m/h]
     * */ 

    double delta;  // the slope of saturation vapor pressure curve (kPa/℃)
    double gamma;  // // psychrometric constant, kPa/℃
    delta = VaporPresSlope(
        Air_tem_avg, Air_tem_min, Air_tem_max
    ); 
    gamma = Const_psychrometric(Air_pres);

    Transpiration = Evapoation_pot * 
        (delta + gamma) / (
            delta + gamma * (1 + Resist_canopy / Resist_aero)
        );
    
    return Transpiration;
}


double VaporPresSlope(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max  /*scalar: maximum air temperature (℃)*/
){
    double delta; // the slope of saturation vapor pressure curve (kPa/℃)

    delta = 4098 * e0(Air_tem_avg) / pow(0.5*(Air_tem_min + Air_tem_max) + 237.3, 2.0);  
    return delta;
}

double Const_psychrometric(
    double Air_pres  // air pressure, kPa
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
    double Air_tem /*scalar: air temperature (celsius degree)*/
) {
    /*
    estimate saturated water-vapor pressures (kPa) from air temperature
    */
    // 根据气温，计算得到瞬时饱和水汽压kPa
    double e0;
    e0 = 0.6108 * exp(17.277 * Air_tem / (Air_tem + 273.3));
    return e0;
}

double Kelvin_tem(
    double tem
) {
    /*concert the temperature from celsius degree to Kelvin K */
    return tem + 273.15;
}

double WET_part(
    double Prec,         /* scalar: precipitation, [m] */
    double Interception, /* scalar: the depth of intercepted water at the start of the time step, [m]*/
    double LAI,
    double Frac_canopy   /* scalar: the fraction of ground surface covered by the canopy*/
){
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


/*************/

void ET_iteration(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_pres,    // air pressure, kPa
    double Air_rhu,     // relative humidity, %
    double Radia_net,   // the net radiation flux density, kJ/h/m2
    
    double Prec,
    double *Interception_o,
    double *Interception_u,
    double Resist_canopy_o,
    double Resist_aero_o,
    /***
     * Resist_aero_o: the aerodynamic resistance to vapor transport
     * between the overstory and the reference height, with the unit of h/m
     */

    double LAI,
    double Frac_canopy,
    double Soil_Fe, //  soil desorptivity
    int Toggle_Understory_BareSoil,
    int step_time // hours
)
{
    double Aw_o, Aw_u;
    double tw; // required to evaporate the intercepted water at the potential rate.
    double Ep_o, Ep_u;
    double Et_o, Et_u;
    double EI_o, EI_u;
    double ET_o, ET_u;
    double ET_s;
    Ep_o = PotentialEvaporation(
        Air_tem_avg, Air_tem_min, Air_tem_max,
        Air_pres, Air_rhu, Radia_net, Resist_aero_o);
    Et_o = Transpiration(
        Ep_o, Air_tem_avg, Air_tem_min, Air_tem_max,
        Air_pres, Resist_canopy_o, Resist_aero_o);

    Aw_o = WET_part(
        Prec, *Interception_o, LAI, Frac_canopy);

    tw = (*Interception_o + Prec) / (Ep_o * Aw_o);
    if (tw <= (double) step_time) {
        /***
         * the overstory intercepted and precipitation 
         *  are evaporated within the time step
        */
        EI_o = (Ep_o * Aw_o) * tw; // overstory evaporation from wet part
        ET_o = Et_o * (1-Aw_o) * tw + // overstory transpiration from wet part
              Et_o * Aw_o * ( (double) step_time - tw); // from dry part
        if ((EI_o + ET_o) >= Ep_o) {
            EI_u = 0.0;
            ET_u = 0.0;
        } else {
            // modified potential evaporation 
            Ep_u = Ep_o - (EI_o + ET_o);
            if (Toggle_Understory_BareSoil == 1)
            {   // the understory (ground) is bare soil
                ET_s = ET_soil(
                    Ep_u, Soil_Fe
                );
            } else {
                /**
                 * the understory is not bare soil, 
                 *      and the soil evaporation is ignored
                */

            }
        }


    } else {
        /****
         * the overstory intercepted water and precipitation are enough for 
         * evaporation within the time step
        */

        EI_o = Ep_o;
        ET_o = 0.0;
        EI_u = 0.0;
        EI_u = 0.0;
        ET_s = 0.0;
    }


}

double ET_soil(
    double ET_soil_pot,
    double Soil_Fe //  soil desorptivity
){
    // remains to be figured out
    /****
     * Soil_Fe: 
     * the soil desorptivity, determined by the rate at 
     *      which the soil can deliver water to the surface. 
     *      Desorptivity is a function of soil type and moisture 
     *      conditions in the upper soil zone.
    */
    double ET_s;
    ET_s = min(ET_soil_pot, Soil_Fe);
    return ET_s;
}

