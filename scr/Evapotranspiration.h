
#ifndef ET_header
#define ET_header

double PotentialEvaporation(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_pres,  // air pressure, kPa
    double Air_rhu,   // relative humidity, %
    double Radia_net, // the net radiation flux density, kJ/h/m2
    double Resist_aero 
    /***
     * Resist_aero: the aerodynamic resistance to vapor transport 
     * between the overstory and the reference height, with the unit of h/m
    */
);

double Transpiration(
    double Evapoation_pot, // appropriate potential evaporation, m/h
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_pres,    // air pressure, kPa
    double Resist_canopy, // aerodynamic resistance to vapor transport, [h/m]
    double Resist_aero    // canopy resistance to vapor transport, [h/m]
);

double VaporPresSlope(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max  /*scalar: maximum air temperature (℃)*/
);

double Const_psychrometric(
    double Air_pres  // air pressure, kPa
);

double e0(
    double Air_tem  /*scalar: air temperature (celsius degree)*/
);

double Kelvin_tem(
    double tem
);

double WET_part(
    double Prec,         /* scalar: precipitation, [m] */
    double Interception, /* scalar: the depth of intercepted water at the start of the time step, [m]*/
    double LAI,
    double Frac_canopy   /* scalar: the fraction of ground surface covered by the canopy*/
);


#endif