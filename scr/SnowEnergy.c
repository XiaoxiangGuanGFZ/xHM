
double FLUX_net_radiation(
    double Radia_Long, // incoming long radiation, [kJ/(m2 * h)]
    double Radia_Short, // incoming short radiation, [kJ/(m2 * h)]
    double Tem_snow,   // temperature of snowpack, [Celsius degree]
    double Snow_albedo // the albedo of snowpack, dimensionless
){
    /* 
    * Net radiation at the snow surface is calculated 
    * given incoming shortwave and longwave radiation 
    */
    double sigma = 4.903 * pow(10, -9); // Stefan-Boltzmann constant MJ/(K4*m2*d)
    double Net_radiation;
    // 1 MJ = 1000 kJ; 1 day = 24 hours
    sigma = sigma * 1000 / 24; // unit: kJ/(K4*m2*h)
    Net_radiation = Radia_Long + Radia_Short * (1 - Snow_albedo) - sigma * pow(Tem_snow + 273.15, 4);
    return Net_radiation;
}

double FLUX_sensible(
    double Tem_air,  // air temperature, [Celsius degree]
    double Tem_snow, // temperature of snowpack, [Celsius degree]
    double Resistance_AirSnow // aerodynamic resistance between the snow surface and the near-surface reference height, [h/m]
){
    /**
     * compute the flux of sensible heat to the snowpack
     * 
    */
   double Density_air = 1.2922;     // air density, kg/m3
   double SpecificHeat_air = 1.005; // specific heat of air, 1.005 kJ/(kg°C) or kJ/(kg * K)
   double Heat_sensible;  // flux of sensible heat to the snow surface, unit: kJ/(m2 * h)
   Heat_sensible = Density_air * SpecificHeat_air * (Tem_air - Tem_snow) / Resistance_AirSnow;
   return Heat_sensible;
}

double Resistance_AirSnow(
    double Windspeed_z, // wind speed at the height of z, [m/s]
    double z,  // the height of record, [m]
    double Depth_snow  // the depth of snowpack, [m]
){
    double z0 = 0.0003; // snow surface roughness, [m]
    double k = 0.4; // von Karman’s constant

    double R_AirSnow; 
    // R_AirSnow is aerodynamic resistance 
    // between the snow surface and the near-surface reference height, [h/m]

    Windspeed_z = Windspeed_z * 3600; // update the unit as [m/h]
    R_AirSnow = pow(log( (z - Depth_snow) / z0 ), 2) / (k*k * Windspeed_z);
    return R_AirSnow;
}

double FLUX_latent(
    double Tem_air,  // air temperature, [Celsius degree]
    double Tem_snow, // temperature of snowpack, [Celsius degree]
    double Pressure_air, // atmospheric pressure, 
    double Rhu,  // relative humidity, [unit: not %]
    double Resistance_AirSnow, // aerodynamic resistance between the snow surface and the near-surface reference height, [h/m]
    int L // whether liquid water exists in the snowpack, yes: 1
){
    double lambda_v = 2500; // latent heat of vaporization, [kJ/kg]
    double lambda_s = 2838; // latent heat of sublimation, [kJ/kg]
    double lambda;
    double Density_air = 1.2922;  // air density, kg/m3
    double Heat_latent; // flux of latent heat to the snow surface, kJ/(m2 * h)
    
    double VaporPressure_air;  // vapor pressure at the measurement height, kPa
    double VaporPressure_snow; // saturated vapor pressure at the snow surface, kPa
    if (Tem_air >= 0.0)
    {
        VaporPressure_air = 0.6108 * exp(17.277 * Tem_air / (Tem_air + 273.3)) * Rhu;
    } else {
        VaporPressure_air = 0.6108 * exp(21.870 * Tem_air / (Tem_air + 265.5)) * Rhu;
    }
    
    if (Tem_snow >= 0.0)
    {
        VaporPressure_snow = 0.6108 * exp(17.277 * Tem_snow / (Tem_snow + 273.3));
    } else {
        VaporPressure_snow = 0.6108 * exp(21.870 * Tem_snow / (Tem_snow + 265.5));
    }
    
    if (L==1)
    {
        /*liquid water is present in the surface layer */
        lambda = lambda_v;
    } else
    {
        lambda = lambda_s;
    }
    
    Heat_latent = lambda * Density_air * 0.622 / Pressure_air * (VaporPressure_air - VaporPressure_snow) / Resistance_AirSnow;
    return Heat_latent;
}

double FLUX_advect(
    double Tem_air,  // air temperature, [Celsius degree]
    double Prec_liq, // input liquid phase water, [m]
    double Prec_sol, // input solid phase water, [m]
    double Time_step  // time interval for each iteration, [hours]
){
    double SpecificHeat_water = 4.22; // the specific heat of water, 4.22 kJ/(kg°C).
    double SpecificHeat_ice = 2.1; // the specific heat of ice, 2.1 kJ/(kg°C).
    double Density_water = 1000;   // water density, kg/m3

    double Heat_advect;  //Advected energy to the snowpack via precipitation (rain or snow), unit: kJ/(m2 * h)
    Heat_advect = (Density_water * SpecificHeat_water * Tem_air * Prec_liq + Density_water * SpecificHeat_ice * Tem_air * Prec_sol) / Time_step;
    return Heat_advect;
}

