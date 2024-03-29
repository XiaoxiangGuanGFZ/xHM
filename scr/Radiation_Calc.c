/*
 * SUMMARY:      Radiation_Calc.c
 * USAGE:        Calculate short and long wave radiations
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Aug-2023
 * DESCRIPTION:  Calculate radiations
 * DESCRIP-END.
 * FUNCTIONS:    
 * 
 * COMMENTS:
 * In order to simulate the snow accumulation and melting process in a physical-based manner,
 * the information about (shortwave and longwave) radiation flux
 * is required as input for energy balance iteration. The radiation flux is either obtained from 
 * instrumental observation or empirically estimated from other meteorological variables, like 
 * air temperature, relative humidity, atmospheric pressure, sunshine duration and wind speed. 
 * 
 * References:
 * Abramowitz G., L. Pouyanné, and H. Ajami (2012). 
 *      On the information content of surface meteorology 
 *      for downward atmospheric long-wave radiation synthesis. 
 *      Geographical Research Letters 39(5), 
 *      doi: https://doi.org/10.1029/2011GL050726
 * 
*/



/******
 * empirical formula for radiation computation:
 * - extraterrestial (shortwave) radiation from sun
 * - longwave radiation 
 * - 
*/


/*******
 * required variables:
 * - date (year-month-day)
 * - latitude (decimal degree, location info)
 * - daily average air temperature, celsius degree (°C)
 * - relative humidity, %
 * - sunshine duration, hours
 * - LAI
 * - forest (canopy) albedo
 * - forest (canopy) fraction, between 0 and 1
 * - snow albedo (assumed as 0.5)
 * 
*/
#include <math.h>
#include "Constants.h"
#include "Radiation_Calc.h"

int NOD(
    int year,
    int month,
    int day
){
    /* number of day */
    /* given a date, derive the number of day,
        1 for 1th January and 365 (or 366) for 31th December.
    */ 
    int NOD;
    static const int days[2][13] = {
        {0, 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
        {0, 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335}
    };
    int leap = 1;
    leap = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);

    NOD = days[leap][month] + day;
    return NOD;
}

/************ shortwave radiation *************/

double Radiation_downward_short(
    int year,
    int month,
    int day,
    double lat,         /* the latitute of the location */ 
    double Air_SSD      /* sunshine duration in a day, hours */ 
){
    /*****
     *  calculate the received shortwave radiation [(MJ⋅m-2⋅d-1)], 
     *  considering the effect of cloudiness, at a daily step
     * 
    */
    double tau_cloud;  // effect of cloud cover on insolation
    double R_et;       // extraterrestrial radiation
    double R_cs;       // clear-sky radiation
    int J; // number of the day
    J = NOD(year, month, day);

    // estimate the extraterrestrial radiation (MJ⋅m-2 d-1)
    double dr, del, w_s, Ra; // intermediate variable

    //the inverse relative distance between earth and sun
    dr = 1 + 0.033 * cos(2 * PI / 365 * J);  

    //the solar decimation (rad)
    del = 0.408 * sin(2 * PI / 365 * J - 1.39); 

    // the sunset hour angle (rad)
    w_s = acos( - tan(lat * PI / 180) * tan(del));
    
    /* R_et: extraterrestrial radiation (MJ⋅m-2 d-1) */
    R_et = 37.59 * dr * (w_s * sin(lat * PI / 180) * sin(del) + cos(lat * PI / 180) * cos(del) * sin(w_s));
    // 37.59: 24(60)/PI*Gsc; Gsc is the solar constant and equals to 0.082MJ⋅m(-2)⋅min(-1)

    R_cs = (as + bs) * R_et;  // clear-sky radiation, non-cloudy
    
    tau_cloud = as + bs * Air_SSD / (24/PI * w_s);  // diminishing effect from cloudiness
    
    return R_cs * tau_cloud; 

}

double Radiation_short_surface(
    double R_sky,  /*the received solar radiation, considering the effect of cloudiness, output from 
     from function Radiation_downward_short() */ 
    double albedo_forest,  // the albedo of the vegetation
    double LAI,            // leaf area index, dimensionless
    double albedo_snow     // the albedo of the snow under the canopy (assumed to be 0.5)
){
    /*****
     * calculate the received shortwave radiation 
     *  on the understory ground (or snow) surface
     * 
    */

    /* applicable scenarios: 
    received shortwave radiation on the SURFACE of
    - canopy: albedo_snow = 0.0
    - canopy snow: albedo_snow = 0.5
    - ground: albedo_snow = 0.0
    - ground snow: albedo_snow = 0.5

    when there is no canopy, albedo_forest = 0.0 and LAI = 0.0
    */

   double tau; // effect of forest canopy and snow on insolation
   tau = (1 - albedo_forest) * (1 - albedo_snow) * exp(-LAI);
   return R_sky * tau; 
}

/************ longwave radiation *************/

double Radiation_long_surface(
    double Tem_air,  /* air temperature, Celsius degree */ 
    double RHU,      /* relative humidity, unit: % */ 
    double FF        /* the fractional forest cover, between 0.0 and 1.0 */ 
){
    /*****
     * calculate the received longwave radiation on the surface
     *      in an empirical manner.
     * Abramowitz et al. (2012) found that 
     *      under both clear and cloudy conditions, 
     *      the received longwave radiation on the surface 
     *      can be estimated with a simple empirical relation 
     *      well without considering the clouds correcting:
    */

    double es;  // saturated vapor pressure, kPa;
    double ea;  // actual vapor pressure, kPa;
    es = 0.6108 * exp(17.277 * Tem_air / (Tem_air + 273.3));
    ea = RHU * es / 100;
    
    double delta = 4.90 * pow(10, -9);  // Stefan-Boltzmann constant, MJ/m2/k4/d
    double Lin;  // downward longwave radiation considering cloudiness, MJ/m2/d
    Lin = 2.7 * ea + 0.245 * (Tem_air + 273.15) - 45.14; // see Abramowitz et al. (2012) 

    /***
     * two part:
     * - canopy covered area: FF * delta * pow(Tem_air + 273.15, 4)
     *      canopy temperature is assumed close to the near-surface air temperature
     * - open area (without canopy cover): Lin * (1 - FF)
     * 
     * */ 
    return FF * delta * pow(Tem_air + 273.15, 4) + Lin * (1 - FF);
}



double Radiation_downward_long(
    int year,
    int month,
    int day,
    double lat,      /* the latitute of the location */ 
    double Tem_air,  /* air temperature, [Celsius degree] */ 
    double RHU,      /* relative humidity, unit: % */ 
    double Air_SSD,  /* sunshine duration in a day, hours */ 
    double FF        /* the fractional forest cover, between 0.0 and 1.0 */ 
){
    /****
     * calculate the received downward longwave radiation,
     *      considering the effect of cloud cover
     * 
     * applicable scenarios:
     * for overstory (canopy): set FF = 0
     * for understory (ground or snow): FF is between 0 and 1.0
     * 
    */
    double delta = 4.90 * pow(10, -9);  // Stefan-Boltzmann constant, MJ/m2/k4/d
    double Lin;  // downward longwave radiation considering cloudiness, MJ/m2/d

    double emissivity_clr, emissivity_sky, emissivity_at;
    /**
     * emissivity_clr: emissivity of clear-sky
     * emissivity_sky: emissivity considering the effect of cloud cover
     * emissivity_at:  emissivity of atmosphere
    */
    double es;  // saturated vapor pressure, kPa;
    double ea;  // actual vapor pressure, kPa;
    es = 0.6108 * exp(17.277 * Tem_air / (Tem_air + 273.3));
    ea = RHU * es / 100;

    emissivity_clr = 0.83 - 0.18 * exp(-1.54 * ea);

    /* calculate the maximum possible duration of sunshine (hours) */
    double dr, del, w_s, N;  
    int J; // number of day
    J = NOD(year, month, day);

    //the inverse relative distance between earth and sun
    dr = 1 + 0.033 * cos(2 * PI / 365 * J);  

    //the solar decimation (rad)
    del = 0.408 * sin(2 * PI / 365 * J - 1.39); 

    // the sunset hour angle (rad)
    w_s = acos( - tan(lat * PI / 180) * tan(del));
    N = 24/PI * w_s;

    emissivity_sky = (1 - Air_SSD / N) + Air_SSD / N * emissivity_clr;
    emissivity_at = (1 - FF) * emissivity_sky + FF;
    
    Lin = emissivity_at * delta * pow(Tem_air + 273.15, 4);
    return Lin;
}





