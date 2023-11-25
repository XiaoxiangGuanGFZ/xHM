
#ifndef RESISTANCE
#define RESISTANCE

/*********** aerodynamic resistance ************/
double Resist_aero_o(
    double Air_ws_obs,   /* wind speed at the measurement height, m/s */
    double ws_obs_z, /* the measurement height, m */
    double zr,       /* reference height, m */
    double Canopy_h, /* height of canopy, m */
    double d,        /* displacement height, m */
    double z0        /* the roughness, m */
);

double Resist_aero_u(
    double Air_ws_obs,  /* wind speed at the measurement height, m/s */
    double ws_obs_z,    /* the measurement height, m */
    double d,           /* displacement height, m */
    double z0           /* roughness length, m */
);

double WindSpeed_Profile(
    double z1, /* height 1, m */
    double z2, /* height 2, m */
    double ws1,/* wind speed at the height 1, m/s */
    double d,  /* displacement height, m */
    double z0  /* roughness lenght, m */
);







/*********** canopy resistance ************/
double Resist_Canopy(
    double rs[],   /* vector: stomatal resistance, h/m */
    double LAI[],  /* vector: LAI for individual leaves (species) */
    int n          /* scalar: number of species */
);

double Resist_Stomatal(
    double Air_tem_avg, /* air temperature (in degrees Celsius) */
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_rhu,     /* relative humidity, % */

    double Rp,     /* the visible radiation */
    double Rpc,    /* the light level where rs is twice the rs_min */
    double rs_min, /* minimum stomatal resistance */
    double rs_max, /* maximum (cuticular) resistance */

    double SM,     /* average soil moisture content */
    double SM_wp,  /* the plant wilting point */
    double SM_free /* the moisture content above which 
                        soil conditions do not
                        restrict transpiration. */
);

double Factor_1(
    double Air_tem_avg  /* air temperature (in degrees Celsius) */
);

double Factor_2(
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_rhu   // relative humidity, %
);

double Factor_3(
    double Rp,     /* the visible radiation */
    double Rpc,    /* the light level where rs is twice the rs_min */
    double rs_min, /* minimum stomatal resistance */
    double rs_max  /* maximum (cuticular) resistance */
);

double Factor_4(
    double SM,     /* average soil moisture content */
    double SM_wp,  /* the plant wilting point */
    double SM_free /* the moisture content above which 
                        soil conditions do not
                        restrict transpiration. */
);





#endif



