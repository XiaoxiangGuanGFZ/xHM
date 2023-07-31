
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
    double Tem_snow,   // temperature of snow surface, [celsius degree]
    double Density_BulkWater, //the bulk density of the liquid water in the snowpack.
    double Wns,   // amounts of newly fallen snow (in water equivalent units, m)
    double Ws,    // amounts of snow on the ground (in water equivalent units, m)
    double Time_step // time interval / time step / simulation temporal step, hours
){
    /**
     * snow densification
     * update the snow density for each time step
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
    if (Ws <= 0.0){
        *Density_snow = 0.0;
    } else {
        /* update the density of an old snowpack */
        Pload = 0.5 * g * Density_water * (Wns + f_com * Ws);
        CR0 = Pload / eta0 * exp(- c5 * (273.15 - Tem_snow)) * exp(-c6 * *Density_snow);

        if (*Density_snow <= 150)
        {
            c3 = 1.0;
        } else {
            c3 = exp(-0.046 * (*Density_snow - 150.0));
        }
        
        if (Density_BulkWater > 0)
        {
            // there is liquid water in snowpack, Density_BulkWater == 1
            c4 = 2;
        } else {
            c4 = 1;
        }
        CRm = 2.788 * 0.000001 * c3 * c4 * exp(-0.04 * (273.15 - Tem_snow));
        *Density_snow = (Time_step * 3600 * (CRm + CR0) + 1) * *Density_snow;
        if (*Density_snow <= 0.0)
        {
            *Density_snow = 0.0;
        } 
    }
}

void SnowMassBalance(
    double Qr,  // net radiation flux, kJ/(m2 * h)
    double Qs,  // sensible radiation flux,
    double Qe,  // latent heat flux,
    double Qp,  // adverted flux to snowpack from precipitation
    double *Tem_snow, // temperature of snowpack, [Celsius degree]
    double *Wliq,  // liquid phase mass in the snowpack, [m]
    double *Wsol,  // solid phase mass in the snowpack, [m]
    double *W,     // SWE (snow water equivalent, m) of the snow pack
    double Prec_liq, // input liquid phase water, [m]
    double Prec_sol, // input solid phase water, [m]
    double Time_step,  // time interval for each iteration, [hours]
    double *Snow_runoff // the SWE generated by melting from snowpack as runoff, [m]
){
    
    double Heat_net; // net energy exchange at the snow surface; 
    double Heat_sum; // Heat_sum = (Qe + Qp + Qs + Qr + Qm) * timestep
    double Heat_liq;
    double Heat_ice;
    double Heat_m; // energy for ice melting or refreezing
    double Qm;     // heat flux for ice melting; unit: kJ/(m2 * h)
    // double lambda_v = 2500; // latent heat of vaporization, [kJ/kg]
    // double lambda_s = 2838; // latent heat of sublimation, [kJ/kg]
    // double lambda_f = 334;  // latent heat of fusion, [kJ/kg]
    // double Density_water = 1000;   // water density, kg/m3
    // double SpecificHeat_ice = 2.1; // the specific heat of ice, 2.1 kJ/(kg°C).
    double C = 0.06; // the liquid water holding capacity coefficient of snowpack
    double d_Wsol;   // changes of solid water volumn (SWE) in the snowpack
    double d_Wliq;   // changes of liquid water (SWE) in the snowpack

    Heat_net = (Qr + Qs + Qe + Qp) * Time_step;
    Heat_liq = Density_water * lambda_f * *Wliq;
    Heat_ice = SpecificHeat_ice * *Wsol * *Tem_snow;
    if (Heat_net < 0.0) {
        /**
         * negative net heat, liquid -> solid phase and refreezing
        */
        // take the minimum value between Heat_liq and (-Heat_net) 
        if ((-Heat_net) >= Heat_liq)
        { 
            /**
             * when Wliq > 0.0, here liquid water is completely condensed to ice (solid phase)
             * when Wliq = 0.0, no liquid water in snowpack, Heat_m = Heat_liq = 0.0 
             */
            Heat_m = Heat_liq;
        } else {
            /**
             * here Wliq > 0.0 and Heat_liq > (- Heat_net), 
             * liquid water (partially) will be condensed to ice.
            */
            Heat_m = -Heat_net;
        }
        // Heat_m is always positive

        Heat_sum = Heat_net + Heat_m;  
    } else {
        /**
         * positive net heat; solid->liquid phase (melting)
         * - when Tem_snow < 0.0 (cold stage); Heat_ice < 0.0
         * - when melting (Tem_snow = 0.0); Heat_ice = 0.0; then Heat_m < 0.0
        */
        Heat_m = -(Heat_net + Heat_ice); 
        /**
         * Heat_m > 0.0, nonmelt period while the Tem_snow tends to increase
         * Heat_m <= 0.0, snow melting
         * */ 
        Heat_sum = Heat_net + Heat_m;
    }
    Qm = Heat_m / Time_step; 

    /* the snowpack mass balance */
    if (*W > 0.0)
    {
        /* snowpack exists */
        if (*Wliq > 0.0)
        {
            /* liquid water exists in the snowpack */
            d_Wliq = Prec_liq + (Qe / lambda_v - Qm / lambda_f) * Time_step / Density_water;
            d_Wsol = Prec_sol + Qm / (Density_water * lambda_f) * Time_step;
        } else {
            /* no liquid water */
            d_Wliq = Prec_liq - Qm / (lambda_f * Density_water) * Time_step;
            d_Wsol = Prec_sol + (Qe / lambda_s + Qm / lambda_f) * Time_step / Density_water;
        }
    }else{
        /* no snowpack */
        // this should be the starting point for a snow event
        d_Wliq = Prec_liq;
        d_Wsol = Prec_sol;
    }
    
    *Wliq = *Wliq + d_Wliq; // update the liquid phase volume in snowpack
    *Wsol = *Wsol + d_Wsol; // solid phase 
    // update snowpack temperature
    if (*Wliq > 0.0) {
        // when snow is melting, the snowpack is isothermatl at 0 celsius degree
        *Tem_snow = 0.0;
    } else if (*Wsol <= 0.0) {
        // snow is gone!
        *Tem_snow = 0.0;
    } else {
        // snow cold process
        *Tem_snow = (*W * *Tem_snow) / *Wsol;
        // *Tem_snow = (Heat_sum / Density_water / SpecificHeat_ice  +  *W * *Tem_snow) / *Wsol;
    }

    if ((*Wliq + *Wsol) > 0.0 && *Wliq > C * (*Wliq + *Wsol))
    {
        // C * *W is the liquid water holding capacity of snowpack, [m]
        *Snow_runoff = *Wliq - C * (*Wliq + *Wsol);  // snow melting runoff
        *Wliq = *Wliq - *Snow_runoff;
    } else {
        *Snow_runoff = 0.0;
    }
    // mass conservation check
    if (*Wliq < 0.0)
    {
        *Wliq = 0.0;
    }

    if (*Wsol < 0.0)
    {
        *Wsol = 0.0;
        *Wliq = 0.0;
    }
    *W = *Wsol + *Wliq;
    
}

/**
 * - design the radiation functions
 * - snow interception process
 * - iteration (time step by time step)
 * - how to output the intermediate variables, at the same time balancing the memory comsumption
 * - startup of snow process, initialize the snow variables
 * 
*/


// preprocessor
// header files
// C functions with variable arguments
// GNU make
// 
