#ifndef PI
#define PI 3.1415925
#endif

#ifndef MAXCHAR
#define MAXCHAR 10000
#endif

#ifndef MAXrows
#define MAXrows 10000
#endif

#ifndef CONSTANTS
#define CONSTANTS

#define VISFRACT 0.5 /* part of shortwave that is in the visible */
// coefficient in terrestrial radiation computation
#define as 0.25
#define bs 0.5

// shortwave reflection coefficient of soil/ground
#define ALBEDO_SOIL 0.159

// rs_max: maximum (cuticular) resistance [50 s/cm = 5000s/m]
#define RS_MAX 5000.0

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

#endif


