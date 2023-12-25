
#ifndef RADIATION
#define RADIATION
/* declare functions from "Radiation_Calc.c" */
int NOD(
    int year,
    int month,
    int day
);


double Radiation_downward_short(
    int year,
    int month,
    int day,
    double lat,         /* the latitute of the location */ 
    double Air_SSD,     /* sunshine duration in a day, hours */ 
    double as,          /* two parameters, 0.25 and 0.5 by default */
    double bs    
);


double Radiation_short_surface(
    double R_sky,  /*the received solar radiation, considering the effect of cloudiness, output from 
     from function Radiation_downward_short() */ 
    double albedo_forest,  /* the albedo of the vegetation */  
    double LAI,            /* leaf area index, dimensionless */ 
    double albedo_snow     /* the albedo of the snow under the canopy (assumed to be 0.5) */ 
);


double Radiation_long_surface(
    double Tem_air,  /* air temperature, Celsius degree */ 
    double RHU,      /* relative humidity, unit: % */ 
    double FF        /* the fractional forest cover, between 0.0 and 1.0 */ 
);


double Radiation_downward_long(
    int year,
    int month,
    int day,
    double lat,      /* the latitute of the location */ 
    double Tem_air,  /* air temperature, Celsius degree */ 
    double RHU,      /* relative humidity, unit: % */ 
    double Air_SSD,  /* sunshine duration in a day, hours */ 
    double FF        /* the fractional forest cover, between 0.0 and 1.0 */ 
);


#endif
