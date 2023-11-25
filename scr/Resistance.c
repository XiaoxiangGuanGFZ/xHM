
#include <math.h>
#include "SnowAccuMelt.h"
#include "Resistance.h"



/*********** aerodynamic resistance ************/

double Resist_aero_o(
    double Air_ws_obs,   /* wind speed at the measurement height, m/s */
    double ws_obs_z, /* the measurement height, m */
    double zr,       /* reference height, m */
    double Canopy_h, /* height of canopy, m */
    double d,        /* displacement height, m */
    double z0        /* the roughness, m */
){
    double Rao;  // aerodynamic resistance, h/m
    double k = 0.4;   // Von Karmen's constant, a dimensionless constant
    double Air_ws_zr; // wind speed at reference height
    double na = 2.5;  // a dimensionless extinction coefficient
    double zw;  // height of the boundary between the upper logarithmic profile and roughness sublayer

    Air_ws_zr = WindSpeed_Profile(
        ws_obs_z, zr, Air_ws_obs,
        d, z0
    );

    zw = 1.5 * Canopy_h - 0.5 * d;
    Rao = log((zr-d)/z0) / (Air_ws_zr * k * k) * \
        (Canopy_h / (na * (zw - d)) * \
        (exp(na * (1 - (d-z0)/Canopy_h)-1)) + \
        (zw-Canopy_h)/(zw - d) + \
        log((zr-d)/(zw-d))
        ); // unit: s/m 
    // 1 h = 60 * 60 s
    return Rao / 3600; // unit: h/m
}

double Resist_aero_u(
    double Air_ws_obs,  /* wind speed at the measurement height, m/s */
    double ws_obs_z,    /* the measurement height, m */
    double d,           /* displacement height, m */
    double z0           /* roughness length, m */
)
{
    /**** function: 
     * aerodynamic resistance for 
     * the soil surface, snow, or understory
     * */ 

    double za;
    double Air_ws_za;
    double Rau;     // aerodynamic resistance, h/m
    double k = 0.4; // Von Karmen's constant, a dimensionless constant

    za = 2 + d + z0;
    Air_ws_za = WindSpeed_Profile(
        ws_obs_z, za, Air_ws_obs,
        d, z0
    );

    Rau = pow(log((za - d)/z0), 2) / (Air_ws_za * k * k); // unit: s/m
    return Rau / 3600;  // unit: h/m
}

double WindSpeed_Profile(
    double z1, /* height 1, m */
    double z2, /* height 2, m */
    double ws1,/* wind speed at the height 1, m/s */
    double d,  /* displacement height, m */
    double z0  /* roughness lenght, m */
)
{
    /* the wind profile above the canopy is assumed to 
    * follow a logarithmic profile
    */
    double ws2;
    ws2 = ws1 * log((z2 - d) / z0) / log((z1 - d) / z0);
    return ws2;
}



/*********** canopy resistance ************/
double Resist_Canopy(
    double rs[],   /* vector: stomatal resistance, h/m */
    double LAI[],  /* vector: LAI for individual leaves (species) */
    int n          /* scalar: number of species */
)
{
    /*****
     * 
     * 
    */
    int i;
    double rc;  // canopy resistance, h/m
    double integral = 0.0;
    double LAI_c = 0.0;
    for (i = 0; i < n; i ++)
    {
        LAI_c += LAI[i];
        integral += LAI[i] / rs[i];
    }
    rc = 1 / integral;

    return rc;
}

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
)
{
    /******
     * the stomatal resistance (of individual leaves) depends on 
     *      vegetation type and environmental factors (
     *      air temperature, vapor pressure deficit,
     *      photosynthetically active radiation flux, and soil moisture
     *      )
    */
    double f1, f2, f3, f4;
    f1 = Factor_1(Air_tem_avg);
    f2 = Factor_2(Air_tem_min, Air_tem_max, Air_rhu);
    f3 = Factor_3(Rp, Rpc, rs_min, rs_max);
    f4 = Factor_4(SM, SM_wp, SM_free);

    double rs;
    rs = rs_min * f1 * f2 * f3 * f4;
    return rs; 
}


double Factor_1(
    double Air_tem_avg  /* air temperature (in degrees Celsius) */
)
{
    double f1;
    f1 = 1 / (
        0.08 * Air_tem_avg - 0.0016 * pow(Air_tem_avg, 2)
    );
    return f1;
}

double Factor_2(
    double Air_tem_min, /*scalar: minimum air temperature (℃)*/
    double Air_tem_max, /*scalar: maximum air temperature (℃)*/
    double Air_rhu   // relative humidity, %
)
{
    double es, ea;
    double ec = 4; // the vapor pressure deficit causing stomatal closure (about 4 kPa)
    double f2;
    es = 0.5 * (e0(Air_tem_max) + e0(Air_tem_min)); 
    /***
     * the function e0() is defined in Evapotranspiration.c
    */
    ea = Air_rhu * es / 100; 
    f2 = 1 / (
        1 - (es - ea) / ec
    );
    return f2;
}

double Factor_3(
    double Rp,     /* the visible radiation */
    double Rpc,    /* the light level where rs is twice the rs_min */
    double rs_min, /* minimum stomatal resistance */
    double rs_max  /* maximum (cuticular) resistance */
)
{
    double f3;
    f3 = ( 1 + Rp/Rpc) / (rs_min / rs_max + Rp/Rpc);
    return f3;
}

double Factor_4(
    double SM,     /* average soil moisture content */
    double SM_wp,  /* the plant wilting point */
    double SM_free /* the moisture content above which 
                        soil conditions do not
                        restrict transpiration. */
)
{
    double f4;
    if (SM <= SM_wp)
    {
        f4 = 0.0;
    } else if (SM <= SM_free)
    {
        f4 = (SM_free - SM_wp) / (SM - SM_wp);
    } else {
        f4 = 1.0;
    }
    return f4;
}


