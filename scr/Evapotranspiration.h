
#ifndef ET_header
#define ET_header

double PotentialEvaporation(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_pres,    /* air pressure, kPa */ 
    double Air_rhu,     /* relative humidity, % */ 
    double Radia_net,   /* the net radiation flux density, kJ/h/m2 */ 
    double Resist_aero
    /***
     * Resist_aero: the aerodynamic resistance to vapor transport
     * between the overstory and the reference height, with the unit of h/m
     */
);

double Transpiration(
    double Evapoation_pot, /* appropriate potential evaporation, m/h */ 
    double Air_tem_avg,    /*scalar: average air tempeature (℃)*/
    double Air_tem_min,    /*scalar: minimum air temperature (℃)*/
    double Air_tem_max,    /*scalar: maximum air temperature (℃)*/
    double Air_pres,       /* air pressure, kPa */ 
    double Resist_canopy,  /* aerodynamic resistance to vapor transport, [h/m] */ 
    double Resist_aero     /* canopy resistance to vapor transport, [h/m] */ 
);

double VaporPresSlope(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max  /*scalar: maximum air temperature (℃)*/
);

double Const_psychrometric(
    double Air_pres // air pressure, kPa
);

double e0(
    double Air_tem /*scalar: air temperature (celsius degree)*/
);

double Kelvin_tem(
    double tem);

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
    double Air_pres,    /* air pressure, kPa */ 

    double Prec_input,        /* precipitation input to the story*/
    double *Prec_throughfall, /* free water leaving the story*/
    double Ep,                /* potential evaporation, m/h */
    double *EI,               /* actual evaporation, m */
    double *ET,               /* actual transpiration, m */
    double *Interception,     /* intercepted water in the story */
    double Resist_canopy,     /* canopy resistance, h/m */
    double Resist_aero,       /* aerodynamic resistance, h/m */
    double LAI,
    double Frac_canopy, /* canopy fraction */
    int step_time       /* hours */ 

);

void ET_iteration(
    double Air_tem_avg, /*scalar: average air tempeature (℃)*/
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_pres,    /* air pressure, kPa */ 
    double Air_rhu,     /* relative humidity, % */ 
    double Radia_net,   /* the net radiation flux density on the cell surface, kJ/h/m2 */ 
    
    double Prec,                /* precipitation (total) within the time step, m */
    double *Prec_throughfall,   /* precipitation throughfall from overstory*/
    double *Prec_net,           /* net precipitation from understory into soil process */
    double *Ep,
    double *EI_o,               /* actual evaporation from overstory, m */
    double *ET_o,               /* actual transpiration from overstory, m */
    double *EI_u,               /* actual evaporation from understory, m */
    double *ET_u,               /* actual transpiration from understory, m */
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
    double Frac_canopy,    /* canopy fraction, [0, 1] */  
    double Soil_Fe,        /* soil desorptivity */  
    int Toggle_Understory, /* 1: there is an understory */ 
    int step_time          /* iteration time step: in hours */ 
 );

void ET_CELL(
    int year,
    int month,
    int day,
    double lat, /* the latitute of the location */

    double Prec,        /* precipitation (total) within the time step, m */
    double Air_tem_avg, /* air temperature (in degrees Celsius) */
    double Air_tem_min, /* scalar: minimum air temperature (℃)*/
    double Air_tem_max, /* scalar: maximum air temperature (℃)*/
    double Air_rhu,     /* relative humidity, unit: % */
    double Air_pres,    /* air pressure, kPa */
    double Air_ws_obs,  /* wind speed at the measurement height, m/s */
    double ws_obs_z,    /* the measurement height, m */
    double n,           /* sunshine duration in a day, hours */

    double as,
    double bs,          /* as and bs: two empirical coefficients, 0.25 and 0.5 by default */
    double *Rno,        /* net radiation for the overstory */
    double *Rno_short,  /* net shortwave radiation for the overstory */
    double *Rnu,        /* net radiation for the understory */
    double *Rnu_short,  /* net shortwave radiation for the understory */
    double *Rns,        /* net radiation for ground/soil */
    double Frac_canopy, /* the fractional forest cover, between 0.0 and 1.0 */
    double Ref_o,       /* reflection coefficient of radiation for overstory */
    double Ref_u,       /* reflection coefficient of radiation for understory */
    double Ref_s,       /* reflection coefficient of radiation for soil/ground */
    double LAI_o,       /* LAI for overstory */
    double LAI_u,       /* LAI for understory */
    double Rpc_o,       /* the light level where rs is twice the rs_min */
    double rs_min_o,    /* minimum stomatal resistance */
    double rs_max_o,    /* maximum (cuticular) resistance */
    double Rpc_u,       /* the light level where rs is twice the rs_min */
    double rs_min_u,    /* minimum stomatal resistance */
    double rs_max_u,    /* maximum (cuticular) resistance */
    double Canopy_zr,   /* reference height of canopy, m */
    double Canopy_h,    /* height of canopy, m */
    double d_o,         /* displacement height of canopy, m */
    double z0_o,        /* the roughness of canopy, m */
    double d_u,         /* displacement height of understory, m */
    double z0_u,        /* roughness length of understory, m */

    double SM,          /* average soil moisture content */
    double SM_wp,       /* the plant wilting point */
    double SM_free,     /* the moisture content above which soil conditions do not restrict transpiration. */
    double Soil_Fe,     /* soil desorptivity */

    double *Prec_throughfall, /* precipitation throughfall from overstory*/
    double *Prec_net,         /* net precipitation from understory into soil process */
    double *Ep,
    double *EI_o,             /* actual evaporation, m */
    double *ET_o,             /* actual transpiration, m */
    double *EI_u,             /* actual evaporation, m */
    double *ET_u,             /* actual transpiration, m */
    double *ET_s,             /* soil evaporation, m */
    double *Interception_o,   /* overstory interception water, m */
    double *Interception_u,   /* understory interception water, m */
    int Toggle_Understory,    
    int step_time             /* iteration time step: in hours */
);

#endif