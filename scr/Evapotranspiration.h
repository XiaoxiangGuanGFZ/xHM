
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


/***** evapotranspiration (ET) ********/
void ET_story(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_pres,    // air pressure, kPa

    double Prec_input,  /* precipitation input to the story*/
    double *Prec_throughfall, /* free water leaving the story*/
    double Ep,                /* potential evaporation, m/h */
    double *EI,               /* actual evaporation, m */
    double *ET,               /* actual transpiration, m */
    double *Interception,     /* intercepted water in the story */
    double Resist_canopy,     /* canopy resistance, h/m */
    double Resist_aero,       /* aerodynamic resistance, h/m */
    double LAI,
    double Frac_canopy,       /* canopy fraction */
    int step_time // hours

);

void ET_iteration(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_pres,    // air pressure, kPa
    double Air_rhu,     // relative humidity, %
    double Radia_net,   // the net radiation flux density, kJ/h/m2
    
    double Prec,                /* precipitation (total) within the time step, m */
    double *Prec_throughfall,   /* precipitation throughfall from overstory*/
    double *Prec_net,           /* net precipitation from understory into soil process */
    double *EI_o,               /* actual evaporation, m */
    double *ET_o,               /* actual transpiration, m */
    double *EI_u,               /* actual evaporation, m */
    double *ET_u,               /* actual transpiration, m */
    double *ET_s,               /* soil evaporation, m */
    double *Interception_o,     /* overstory interception water, m */
    double *Interception_u,     /* understory interception water, m */
    double Resist_canopy_o,
    double Resist_canopy_u,
    double Resist_aero_o,
    double Resist_aero_u,
    /***
     * Resist_aero_o: the aerodynamic resistance to vapor transport
     * between the overstory and the reference height, with the unit of h/m
     */

    double LAI_o,
    double LAI_u,
    double Frac_canopy,   //  canopy fraction, [0, 1]
    double Soil_Fe,       //  soil desorptivity
    int Toggle_Overstory,
    // whether there is overstory, yes: 1
    int Toggle_Understory_BareSoil,
    // whether the understory is bare soil, yes: 1
    int step_time         // iteration time step: in hours
 );



#endif