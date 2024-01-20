#ifndef FUNC_TEM
#define FUNC_TEM

double VaporPresSlope(
    double Air_tem_avg, /* scalar: average air tempeature (℃) */
    double Air_tem_min, /* scalar: minimum air temperature (℃)*/
    double Air_tem_max  /* scalar: maximum air temperature (℃)*/
);

double Const_psychrometric(
    double Air_pres     /* air pressure, kPa */ 
);

double e0(
    double Air_tem      /* scalar: air temperature (celsius degree) */
);

double Kelvin_tem(
    double tem);

#endif