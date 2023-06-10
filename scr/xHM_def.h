
#define PI 3.1415926
#define ALBEDO 0.23  //canopy reflection coefficient

typedef struct 
{
    /* data */
    int acctNum;
    char lastName[15];
    char firstName[20];
    double balance;
} clientData;


double penman(
    double Ta,   /*scalar: daily average air tempeature (celsius degree)*/
    double Tmax, /*scalar: daily maximum air temperature (celsius degree)*/
    double Tmin, /*scalar: daily minimum air temperature (celsius degree)*/
    double RH,   /*scalar: relative humidity (%)*/
    double Wv,   /*scalar: wind speed (m/s)*/
    double Pa,   /*scalar: atmospheric pressure (kPa)*/
    double n,    /*scalar: daylight duration (hours)*/
    double J,       /*scalar: the position of day in a year*/
    double lat      /*scalar: latitude (decimal degree)*/
);

double e0(
    double Ta /*scalar: air temperature (celsius degree)*/
);

double U_2(
    double Wv,
    double height
);

double Kelvin_tem(
    double tem
);

#include "penman.c"
