#define PI 3.1415925


int NOD(
    int year,
    int month,
    int day
);


double Radiation_downward_short(
    int year,
    int month,
    int day,
    double lat,  // the latitute of the location
    double n,   // sunshine duration in a day, hours
    double albedo_forest,  //  the albedo of the vegetation
    double LAI,
    double albedo_snow,    // the albedo of the snow under the canopy (assumed to be 0.5)
    // two parameters, 0.25 and 0.5 by default
    double as,
    double bs    
);



double Radiation_downward_long(
    int year,
    int month,
    int day,
    double lat,  // the latitute of the location
    double Tem_air,  // air temperature, Celsius degree
    double RHU,   // relative humidity, unit: %
    double n,  // sunshine duration in a day, hours
    double FF  // the fractional forest cover, between 0.0 and 1.0
);


#include "Radiation_Calc.c"

