/*
 * SUMMARY:      Route_Channel.c
 * USAGE:        Calculate the flow routing in the river channel (reach)
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  Calculate the flow routing process in the 
 *               river channels (segments or reaches), with a 
 *               linear storage method
 * DESCRIP-END.
 * FUNCTIONS:    
 * 
 * COMMENTS:
 * 
 * 
 * REFERENCES:
 *  
 * 
 */

/******************************************************************
 * VARIABLES:
 * double *Qin             - inflow of the river reach, [m3/h]
 * double *Qout            - outflow of the river reach, [m3/h]
 * double *Vin             - water volume of river reach at the beginning of the time interval, [m3]
 * double *Vout            - water volume of river reach after the time interval, [m3]
 * double k                - storage parameter: equal to the inverse of the average residence time, [1/h]
 * int step_time           - time step, interval, [h]
 * 
 * 
******************************************************************/

#include <math.h>
#include "Route_Channel.h"

void Channel_Routing(
    double *Qin,
    double *Qout,
    double *Vin,
    double *Vout,
    double k,
    int step_time
)
{
    *Vout = *Qin / k + 
        (*Vin - *Qin / k) * exp(- k * (double) step_time);
    *Qout = *Qin - (*Vout - *Vin) / (double) step_time;

}