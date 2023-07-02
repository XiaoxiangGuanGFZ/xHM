
void Partition_RainSnow(
    double *Prec,    // precipitation, [depth unit],
    double *Tem_air, // air temperature, [celsius degree]
    double *Prec_rain, // rainfall, [depth unit]
    double *Prec_snow // snowfall, [depth unit]
){
    /*threshold air temperatures in rain-snow fall partition*/
    double Tem_min = -1.1;
    double Tem_max = 3.3; 
    if (*Tem_air <= Tem_min)
    {
        *Prec_snow = *Prec;
    } else if (*Tem_air <= Tem_max)
    {
        *Prec_snow = *Prec * (Tem_max - *Tem_air) / (Tem_max - Tem_min);
    } else {
        *Prec_snow = 0.0;
    }
    *Prec_rain = *Prec - *Prec_snow;
    
}

void SnowAlbedo(
    double *Albedo,  // the albedo of snow (under specific season)
    double t,  // the time since the last snowfall (in hours)
    int stage  // season indicator (1: snow accumulation season) 
){
    double lambda_a = 0.92;  // parameter for snow accumulation season
    double lambda_m = 0.70;  // parameter for snow melting season
    if (stage == 1) 
    {
        /*accumulation season*/
        *Albedo = 0.85 * pow(lambda_a, pow(t / 24, 0.58));
    } else {
        *Albedo = 0.85 * pow(lambda_m, pow(t / 24, 0.46));
    }
}

void SnowDensity(
    double *Density_snow,  // snow density, kg/m3
    double *Depth_snow,  // the depth of snowpack, [m]
    double *Tem_snow,   // temperature of snow surface, [celsius degree]
    double Density_BulkWater, //the bulk density of the liquid water in the snowpack.
    double Wns,   //  amounts of newly fallen snow (in water equivalent units, m)
    double Ws,    // amounts of snow on the ground (in water equivalent units, m)
    double time_interval // time interval / time step, hours
){
    /**
     * snow densification
     * update the snow density and snow depth of the ground 
     * snowpack for each time step
    */
    double c3;
    double c4;
    double c5 = 0.08;  // unit: k-1
    double c6 = 0.021; // unit: m3/kg
    double eta0 = 3.6*1000000; // the viscosity coefficient at 0 degree, unit: N s/m2
    double Pload; //load pressure
    double f_com = 0.6; //  internal compaction rate coefficient
    double CR0;
    double CRm;

    Pload = 0.5 * g * DensityWater * (Wns + f_com * Ws);
    CR0 = Pload / eta0 * exp(- c5 * (273.15 - *Tem_snow)) * exp(-c6 * *Density_snow);

    if (*Density_snow <= 150)
    {
        c3 = 1.0;
    } else {
        c3 = exp(-0.046 * (*Density_snow - 150.0));
    }
    
    if (Density_BulkWater > 0)
    {
        c4 = 2;
    } else {
        c4 = 1;
    }
    CRm = 2.778 * 0.000001 * c3 * c4 * exp(-0.04 * (273.15 - *Tem_snow));
    *Density_snow = (time_interval * (CRm + CR0) + 1) * *Density_snow ;
    if (*Density_snow <= 0.0)
    {
        *Density_snow = 0.0;
        *Depth_snow = 0.0;
    } else {
        *Depth_snow = Ws * DensityWater / *Density_snow;
    }

}