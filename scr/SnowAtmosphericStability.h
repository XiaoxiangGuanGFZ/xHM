
#ifndef SNOW_ATMOS_STA
#define SNOW_ATMOS_STA
/*
SnowAtmosphericStability.c
correct the Aerodynamic Resistance considering the atmospheric stability
*/
double RichardsonNumber(
    double Tem_Air,  // air temperature, [celsius degree]
    double Tem_snow,  // temperature of snow surface, [celsius degree]
    double WindSpeed_z, // wind speed at the height of z, 
    double z         // the height of the layer where the air temperature and wind speed are recored
);

double AerodynamicResistance(
    double r_as, // the aerodynamic resistance between the snow surface and the near-surface reference height
    double Ri_b // Richardson’s number
);

#endif