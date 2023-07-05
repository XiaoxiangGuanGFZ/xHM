#define g 9.80  // acceleration of gravity, 9.8 m/s2
#define DensityWater 1000 // density of (liquid) water, 1000 kg/m3
#define Density_air
#define Density_ice
#define lambda_v  // latent heat 
#define lambda_s
#define lambda_f
#define SpecificHeat_air
#define SpecificHeat_ice
#define SpecificHeat_water

#include <math.h>


/*
correct the Aerodynamic Resistance considering the atmospheric stability
*/
double RichardsonNumber(
    double Tem_Air,  // air temperature, [celsius degree]
    double Tem_snow,  // temperature of snow surface, [celsius degree]
    double WindSpeed_z, // wind speed at the height of z, 
    double z         // the height of the layer where the air temperature and wind speed are recored
);

void AerodynamicResistance(
    double *r_as, // the aerodynamic resistance between the snow surface and the near-surface reference height
    double Ri_b, // Richardson’s number
);

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
)


#include "SnowAtmosphericStability.c"
#include "SnowInterception.c"
#include "SnowAccuMelt.c"


