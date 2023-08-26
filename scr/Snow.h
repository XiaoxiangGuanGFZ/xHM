/*
define some global (environment) variables
*/
#undef  PI
  #define PI 3.1415925

// #define PI 3.1415925
#define g 9.80  // acceleration of gravity, 9.8 m/s2
#define Density_water 1000   // density of (liquid) water, 1000 kg/m3
#define Density_air  1.2922  // air density, kg/m3
#define Density_ice  917     // ice density, kg/m3
#define lambda_v  2500 // latent heat of vaporization, [kJ/kg]
#define lambda_s  2838 // latent heat of sublimation, [kJ/kg]
#define lambda_f  334  // latent heat of fusion, [kJ/kg]
#define SpecificHeat_air 1.005  //specific heat of air, 1.005 kJ/(kg°C)
#define SpecificHeat_ice 2.1    // the specific heat of ice, 2.1 kJ/(kg°C).
#define SpecificHeat_water 4.22 // the specific heat of water 4.22 kJ/(kg°C).


#include <math.h>


// declare the self-defined functions

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


/*
SnowAccuMelt.c
main process in snow accumulation and melting
*/
void Partition_RainSnow(
    double *Prec,    // precipitation, [depth unit],
    double *Tem_air, // air temperature, [celsius degree]
    double *Prec_rain, // rainfall, [depth unit]
    double *Prec_snow // snowfall, [depth unit]
);

void SnowAlbedo(
    double *Albedo,  // the albedo of snow (under specific season)
    double t,  // the time since the last snowfall (in hours)
    int stage  // season indicator (1: snow accumulation season) 
);

void SnowDensity(
    double *Density_snow,  // snow density, kg/m3
    // double *Depth_snow,  // the depth of snowpack, [m]
    double Tem_snow,   // temperature of snow surface, [celsius degree]
    double Density_BulkWater, //the bulk density of the liquid water in the snowpack.
    double Wns,   //  amounts of newly fallen snow (in water equivalent units, m)
    double Ws,    // amounts of snow on the ground (in water equivalent units, m)
    double time_interval // time interval / time step, hours
);

double CanopySnowCapacity(
    double Tem_air,  // ambient air temperature, degree Celsius
    double LAI       // single-sided leaf-area-index of canopy
);

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
);


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




// include the customized functions in 

#include "SnowEnergy.c"
#include "SnowAtmosphericStability.c"
#include "SnowInterception.c"
#include "SnowAccuMelt.c"


