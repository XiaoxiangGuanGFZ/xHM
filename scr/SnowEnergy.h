
#ifndef SNOW_ENERGY

#define SNOW_ENERGY

/*
SnowEnergy.c
energy balance compoments near the snow surface
*/

double FLUX_net_radiation(
    double Radia_Long, // incoming long radiation, [kJ/(m2 * h)]
    double Radia_Short, // incoming short radiation, [kJ/(m2 * h)]
    double Tem_snow,   // temperature of snowpack, [Celsius degree]
    double Snow_albedo // the albedo of snowpack, dimensionless
);


double FLUX_sensible(
    double Tem_air,  // air temperature, [Celsius degree]
    double Tem_snow, // temperature of snowpack, [Celsius degree]
    double Resistance_AirSnow // aerodynamic resistance between the snow surface and the near-surface reference height, [h/m]
);

double Resistance_AirSnow(
    double Windspeed_m, // wind speed at the height of z, [m/s]
    double z_m,  // the height of record, [m]
    double Depth_snow  // the depth of snowpack, [m]
);

double Resistance_aero_canopy(
    double Windspeed_m,  // wind speed at the height of z_m, [m/s]
    double z_m,  // the height of wind speed measurement, usually 10 [m]
    double zd,   // zero-plane displacement height, [m]
    double z0    // roughness height of canopy, [m]
);

double FLUX_latent(
    double Tem_air,  // air temperature, [Celsius degree]
    double Tem_snow, // temperature of snowpack, [Celsius degree]
    double Pressure_air, // atmospheric pressure, 
    double Rhu,  // relative humidity, [unit: %]
    double Resistance_AirSnow, // aerodynamic resistance between the snow surface and the near-surface reference height, [h/m]
    int L // whether liquid water exists in the snowpack, yes: 1
);

double FLUX_advect(
    double Tem_air,  // air temperature, [Celsius degree]
    double Prec_liq, // input liquid phase water, [m]
    double Prec_sol, // input solid phase water, [m]
    double Time_step  // time interval for each iteration, [hours]
);


#endif