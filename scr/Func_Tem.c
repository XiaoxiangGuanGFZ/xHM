/*
 * SUMMARY:      Func_Tem.c
 * USAGE:        air temperature related functions
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Nov-2023
 * DESCRIPTION:  Calculate air temperature related properties
 * DESCRIP-END.
 * FUNCTIONS:    VaporPresSlope(), Const_psychrometric(), e0(), Kelvin_tem()
 * 
 * COMMENTS:
 * 
 * REFERENCES:
 *
 */

#include <stdio.h>
#include <math.h>
#include "Func_Tem.h"

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
