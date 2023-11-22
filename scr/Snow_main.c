#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "Snow_GVAR.h"
#include "Radiation_Calc.h"
#include "SnowEnergy.h"
#include "SnowAccuMelt.h"
#include "SnowAtmosphericStability.h"


#define MAXCHAR 3000
#define MAXrow 15000  // almost 100 years long ts

struct Date {
    int y;
    int m;
    int d;
};

typedef struct 
{
    /***
     * data structure including following elements:
     * global parameter controlling the behavior of the program
     */

    int START_YEAR;     // year of the starting date
    int START_MONTH;    // month of the starting date
    int START_DAY;      // day of the starting date
    int STEP;           // time step, unit:hour; by default STEP = 24 (daily)

    char FILE_PATH[200];        // file path of input weather data
    char FILE_OUT[200];         // file path of the output snow simulations
    char FILE_OUT_flux[200];    // file path of the output energy flux estimations
    
    double WIND_HEIGHT;         // the height of wind speed measurement, usually 10 [m]
    int N_COL;          // number of columns in weather input file  
    double NA;          // not available (NA) notation, be default -99.9
    char VAR[11][20];   // the variable list provided in the weather input file
} Struct_Para_global;

typedef struct 
{
    struct Date date;
    double PREC;        //precipitation, mm
    double RAINFALL;        // rainfall, mm (liquid phase)
    double SNOWFALL;        // snowfall, mm (solid phase)
    int SNOWFALL_Interval;  //the time since the last snowfall (in days)
    double WINSD;           // wind speed, m/s
    double TEM_AIR_AVG;     // air temperature, celsius degree
    double RHU;             // relative humidity, %
    double SUNDUR;          // sunshine duration, hour 
    double AIRPRE;          // atmospheric pressure, kPa
    double R_SHORT;         // short-wave radiation, unit: MJ/m2 in one STEP, 
    double R_LONG;          // long-wave radiation
    /****
     * unit conversion: MJ/m2/d = 1000/24 (kJ/m2/h)
     * if modelling daily processes, STEP = 24; 
     *      then the unit should be MJ/m2/d in the weather input data file;
     * if STEP = 1 or 2 hours,
     *      then the unit should be carefully modified during input preparation
     * */ 
} Struct_Meteo;


typedef struct 
{
    // struct Date date; 
    double INPUT_rain;
    double INPUT_snow; 
    double SNOW_DEPTH;      // depth, unit: m
    double W;   // snow water equivalent (m)
    double Wice;        // solid phase in snow pack, unit: m
    double Wliq;        // liquid phase in snow pack, unit: m
    double SNOW_ALBEDO;     // albedo of snow surface
    double SNOW_RUNOFF;  // excess melting/rainfall water (liquid phase) from snowpack
    double SNOW_TEM;        // the temperature of snowpack
    double SNOW_DENSITY;    // the density of snowpack
    double SNOW_Ras;     // aerodynamic resistance
    double MASS_Release;     // Mass release (liquid phase) from snowpack
    double THROUGHFALL_rain; // excess throughfall rain
} Struct_Snow;

typedef struct
{   
    // unit: kJ/(m2 * h)
    double net_radiation;
    double sensible;
    double latent;
    double advect;
    double Heat_PhaseChange; // heat of phase change of snowpack
} Struct_snow_flux;

typedef struct
{   
    int GRIDid;
    double LAT;     // latitude 
    double LAI;     // single-side Leaf-Area-Index
    double LAI2;    // all-side LAI
    double zd;      // zero-plane displacement height (m) of canopy
    double z0;      // roughness height (m)
    double ALBEDO_CANOPY;   // canopy albedo
    double FOREST_FRAC;     // canopy fraction
    double Canopy_snow_c;   // maximum interception capacity of canopy snow, [m]
} Struct_surface;

/* functions declaration */
void import_global(
    char fname[], 
    Struct_Para_global *p_gp,
    Struct_surface *p_surface
);  
int import_Meteo(
    Struct_Para_global *p_gp,
    Struct_Meteo *P_meteo
);
void FLUX_zero(
    Struct_snow_flux *p_df_flux
);
void Initialize_snow(
    Struct_Snow *p_df_snow
);

int main(int argc, char * argv[]) {
    /*
    int argc: the number of parameters of main() function;
    char *argv[]: pointer array
    */
    /* char fname[100] = "D:/xHM/example_data/Snow_GlobalParas.txt";
        this should be the only extern input for this program */

    Struct_Para_global GlobalPara = {  // initialize
        1990,1,1,24,
        "example.txt",
        "example_output.txt",
        "example_output_flux.txt",
        10.0,
        7,
        -99.9,
        {"SKIP", "SKIP", "SKIP", "WINSD1", "TEM_AIR_AVG1", "RHU1", "SUNDUR1"}
    };
    
    Struct_Para_global *p_gp;  // pointer to struct GlobalPara
    p_gp = &GlobalPara;     
    
    Struct_surface df_surface = {2596, 40.27, 2.5, 1.50, 1.5, 0.9, 0.086, 0.5, 0.0};

    /******* import the global parameters ***********
    parameter from main() function, pointer array
    argv[0]: pointing to the first string from command line (the executable file)
    argv[1]: pointing to the second string (parameter): file path and name of global parameter file.
    */
    import_global(*(++argv), p_gp, &df_surface);
    
    printf("----- import global parameters ------- \n");
    printf("START_YEAR: %d\nSTART_MONTH: %2d\nSTART_DAY: %2d\nSTEP: %d\n",
        p_gp->START_YEAR, p_gp->START_MONTH, p_gp->START_DAY, p_gp->STEP
        );
    printf("LAT: %.2f\nLAI: %.2f\nALBEDO_CANOPY: %.2f\nFOREST_FRAC: %.2f\nWIND_HEIGHT: %.2f\n",
        df_surface.LAT, df_surface.LAI, df_surface.ALBEDO_CANOPY, df_surface.FOREST_FRAC, p_gp->WIND_HEIGHT
        );
    printf(
        "FILE_PATH: %s\nFILE_OUT: %s\nFILE_OUT_RADIA: %s\n",
        p_gp->FILE_PATH, p_gp->FILE_OUT, p_gp->FILE_OUT
    );

    Struct_Meteo TS_Meteo[MAXrow];
    int nrow = 0; // number of rows in the data file
    nrow = import_Meteo(p_gp, TS_Meteo);
    printf("number of rows: %d\n", nrow);

    /* check (print) the meteo data */
    int i;
    printf("PREC(mm)\tAirTEM\tRHU\tWINSD\tSUNDUR\tR_long\tR_short\n");
    printf("----------------------------------------------------------\n");
    for (i = 0; i < 20; i++) 
    {
        printf("%.1f\t%.1f\t%.0f\t%.1f\t%.1f\t%.2f\t%.2f\n", 
        (TS_Meteo+i)->PREC, (TS_Meteo+i)->TEM_AIR_AVG, 
        (TS_Meteo+i)->RHU,
        (TS_Meteo+i)->WINSD, (TS_Meteo+i)->SUNDUR,
        (TS_Meteo+i)->R_LONG, (TS_Meteo+i)->R_SHORT
        );
    }
    
    Struct_Snow df_snow; 
    Struct_Snow df_snowcanopy; 
    Initialize_snow(&df_snow);
    Initialize_snow(&df_snowcanopy);
    
    Struct_snow_flux df_flux; 
    Struct_snow_flux df_fluxcanopy; 
    FLUX_zero(&df_flux);  // initialize fluxes of snowpack
    FLUX_zero(&df_fluxcanopy);

    /* separate the rainfall and snowfall from precipitation */
    for (i = 0; i < nrow; i++){
        
        Partition_RainSnow(
            &(TS_Meteo+i)->PREC,
            &(TS_Meteo+i)->TEM_AIR_AVG,
            &(TS_Meteo+i)->RAINFALL,
            &(TS_Meteo+i)->SNOWFALL
        );
        if (i == 0) {
            /*****
             * initialize the SNOWFALL_Interval,
             * which is used to estimate how old is the snowpack, as 
             * an influential factor on snow albedo
             * */ 
            TS_Meteo->SNOWFALL_Interval = 1;
        } else {
            if ((TS_Meteo + i)->SNOWFALL <= 0.0){
                // no snowfall 
                (TS_Meteo + i)->SNOWFALL_Interval = (TS_Meteo + i-1)->SNOWFALL_Interval + p_gp->STEP/24;
            } else {
                (TS_Meteo + i)->SNOWFALL_Interval = 1;
            }
            
        }
        // printf("%6.2f\t%6.2f\n", (TS_Meteo+i)->RAINFALL, (TS_Meteo+i)->SNOWFALL);
    }

    /* initialize the output (file path and header) */
    FILE *f_output;
    if ((f_output=fopen(p_gp->FILE_OUT, "w")) == NULL) {
        printf("Program terminated: cannot create or open output file\n");
        exit(0);
    }
    FILE *f_output_flux;
    if ((f_output_flux=fopen(p_gp->FILE_OUT_flux, "w")) == NULL) {
        printf("Program terminated: cannot create or open output file\n");
        exit(0);
    }

    fprintf(
        f_output,
        "y,m,d,SNOW_DEPTH,W,Wice,Wliq,SNOW_RUNOFF,SNOW_ALBEDO,SNOW_DENSITY,SNOW_Ras,SNOW_TEM\n"
    );
    fprintf(
        f_output_flux,
        "y,m,d,net_radiation,sensible,latent,advect\n"
    );

    /* snow simulation (iteration) */
    for (i = 0; i < nrow; i++)
    {
        /*---canopy snow---*/
        df_surface.Canopy_snow_c = CanopySnowCapacity((TS_Meteo + i)->TEM_AIR_AVG, df_surface.LAI);
        df_snowcanopy.INPUT_rain = (TS_Meteo+i)->RAINFALL / 1000 * df_surface.FOREST_FRAC;
        
        if ((TS_Meteo+i)->SNOWFALL > 0.0) {
           df_snowcanopy.INPUT_snow = (TS_Meteo+i)->SNOWFALL / 1000 * 0.6 * df_surface.FOREST_FRAC;
           df_snow.INPUT_snow = (TS_Meteo+i)->SNOWFALL / 1000 * (1 - 0.6 * df_surface.FOREST_FRAC);
        } 
        else
        {
            df_snowcanopy.INPUT_snow = 0.0;
            df_snow.INPUT_snow = 0.0;
        }
        if (df_snowcanopy.W == 0.0 && df_snowcanopy.INPUT_snow > 0.0) {
            /**
             * no snow on the canopy, while there is a snowfall coming
            */
            SnowMassBalance(
                0.0,                             // double Qr,  // net radiation flux, kJ/(m2 * h)
                0.0,                             // double Qs,  // sensible radiation flux,
                0.0,                             // double Qe,  // latent heat flux,
                0.0,                             // double Qp,  // adverted flux to snowpack from precipitation
                &df_snowcanopy.SNOW_TEM,         // double *Tem_snow, // temperature of snowpack, [Celsius degree]
                &df_snowcanopy.Wliq,             // double *Wliq,  // liquid phase mass in the snowpack, [m]
                &df_snowcanopy.Wice,             // double *Wsol,  // solid phase mass in the snowpack, [m]
                &df_snowcanopy.W,                // double *W,     // SWE (snow water equivalent, m) of the snow pack
                df_snowcanopy.INPUT_rain,        // double Prec_liq, // input liquid phase water, [m]
                df_snowcanopy.INPUT_snow,        // double Prec_sol, // input solid phase water, [m]
                p_gp->STEP,                      // double Time_step,  // time interval for each iteration, [hours]
                &df_snowcanopy.SNOW_RUNOFF,      // double *Snow_runoff, // the SWE generated by melting from snowpack as runoff, [m]
                &df_snowcanopy.MASS_Release,     // double *MASS_release, // mass release from snowpack (canopy)
                &df_snowcanopy.THROUGHFALL_rain, // double *THROUGHFALL_rain
                df_surface.Canopy_snow_c,        // double Canopy_snow_c;    // maximum interception capacity of canopy snow, [m]
                df_surface.LAI2,                 // double LAI2, // all-sided leaf-area-index
                0                                // int G_or_C   // indicator, 1: ground snow; 0: canopy snow
            );
            df_snowcanopy.SNOW_TEM = (TS_Meteo + i)->TEM_AIR_AVG;
            // here, density of newly fallen (fresh) snow, kg/m3
            df_snowcanopy.SNOW_DENSITY = 67.92 + 51.25 * exp((TS_Meteo + i)->TEM_AIR_AVG / 2.59);
            df_snowcanopy.SNOW_ALBEDO = 0.9; // the albedo of freshly fallen snow
            df_snowcanopy.SNOW_DEPTH = SNOW_depth(df_snowcanopy.SNOW_DENSITY, df_snowcanopy.W);
            df_snowcanopy.SNOW_Ras = 0.01;
            /* energy flux of snowpack */
            FLUX_zero(&df_fluxcanopy);
        } 
        else if (df_snowcanopy.W > 0.0)
        {
            /* snowpack: energy flux */
            df_snowcanopy.SNOW_Ras = Resistance_aero_canopy(
                (TS_Meteo + i)->WINSD, p_gp->WIND_HEIGHT, df_surface.zd, df_surface.z0);
            if ((TS_Meteo + i)->TEM_AIR_AVG > 0.0)
            {
                // melting season, update the Ras, considering the atmospheric stability
                df_snowcanopy.SNOW_Ras = AerodynamicResistance(
                    df_snowcanopy.SNOW_Ras,
                    RichardsonNumber(
                        (TS_Meteo + i)->TEM_AIR_AVG,
                        df_snowcanopy.SNOW_TEM,
                        (TS_Meteo + i)->WINSD, p_gp->WIND_HEIGHT));
            }
            df_fluxcanopy.net_radiation = FLUX_net_radiation(
                (TS_Meteo + i)->R_LONG * 1000 / p_gp->STEP,     // convert unit to kJ/(m2 * h)
                (TS_Meteo + i)->R_SHORT * 1000 / p_gp->STEP,
                df_snowcanopy.SNOW_TEM,
                df_snowcanopy.SNOW_ALBEDO);  // unit of net radiation: kJ/(m2 * h)
            df_fluxcanopy.sensible = FLUX_sensible(
                (TS_Meteo + i)->TEM_AIR_AVG,
                df_snowcanopy.SNOW_TEM,
                df_snowcanopy.SNOW_Ras);
            df_fluxcanopy.latent = FLUX_latent(
                (TS_Meteo + i)->TEM_AIR_AVG,
                df_snowcanopy.SNOW_TEM,
                (TS_Meteo + i)->AIRPRE,
                (TS_Meteo + i)->RHU,
                df_snowcanopy.SNOW_Ras,
                (df_snowcanopy.Wliq > 0.0) ? 1 : 0 // whether liquid water exists in the snowpack, yes: 1
            );
            df_fluxcanopy.advect = FLUX_advect(
                (TS_Meteo + i)->TEM_AIR_AVG,
                df_snowcanopy.INPUT_rain, 
                df_snowcanopy.INPUT_snow,
                p_gp->STEP);

            /* snowpack: mass balance */
            SnowMassBalance(
                df_fluxcanopy.net_radiation,
                df_fluxcanopy.sensible,
                df_fluxcanopy.latent,
                df_fluxcanopy.advect,
                &df_snowcanopy.SNOW_TEM,         // double *Tem_snow, // temperature of snowpack, [Celsius degree]
                &df_snowcanopy.Wliq,             // double *Wliq,  // liquid phase mass in the snowpack, [m]
                &df_snowcanopy.Wice,             // double *Wsol,  // solid phase mass in the snowpack, [m]
                &df_snowcanopy.W,                // double *W,     // SWE (snow water equivalent, m) of the snow pack
                df_snowcanopy.INPUT_rain,        // double Prec_liq, // input liquid phase water, [m]
                df_snowcanopy.INPUT_snow,        // double Prec_sol, // input solid phase water, [m]
                p_gp->STEP,                      // double Time_step,  // time interval for each iteration, [hours]
                &df_snowcanopy.SNOW_RUNOFF,      // double *Snow_runoff, // the SWE generated by melting from snowpack as runoff, [m]
                &df_snowcanopy.MASS_Release,     // double *MASS_release, // mass release from snowpack (canopy)
                &df_snowcanopy.THROUGHFALL_rain, // double *THROUGHFALL_rain
                df_surface.Canopy_snow_c,        // double Canopy_snow_c;    // maximum interception capacity of canopy snow, [m]
                df_surface.LAI2,                 // double LAI2, // all-sided leaf-area-index
                0                                // int G_or_C   // indicator, 1: ground snow; 0: canopy snow
            );
            /* update the snow property: density, albedo, depth */
            SnowDensity(
                &(df_snowcanopy.SNOW_DENSITY),
                df_snowcanopy.SNOW_TEM,
                (df_snowcanopy.Wliq > 0.0) ? 1 : 0, // Density_BulkWater, whether there is liquid water in snowpack
                df_snowcanopy.INPUT_snow,    // unit: m
                df_snowcanopy.W,
                p_gp->STEP);
            df_snowcanopy.SNOW_DEPTH = SNOW_depth(df_snowcanopy.SNOW_DENSITY, df_snowcanopy.W);
            SnowAlbedo(
                &df_snowcanopy.SNOW_ALBEDO,
                (TS_Meteo + i)->SNOWFALL_Interval,
                ((df_snowcanopy.Wliq > 0.0) ? 0 : 1) // 1: snow accumulation season; 0: snow melting season
            );
            // printf("%6.1f\t%6.2f\t%6.3f\n",(TS_Meteo+i)->WINSD, df_snow.SNOW_DEPTH, df_snow.SNOW_Ras);
            
        } else {
            /******
                df_snow.W == 0.0 && (TS_Meteo+i)->SNOWFALL == 0.0, 
                nothing related to snow process happens
            */
           Initialize_snow(&df_snowcanopy);
           df_snowcanopy.SNOW_RUNOFF = df_snowcanopy.INPUT_rain;           
           FLUX_zero(&df_fluxcanopy); // zero energy flux on snowpack
        }

        /*---ground snow---*/
        df_snow.INPUT_rain = (TS_Meteo+i)->RAINFALL / 1000 * (1-df_surface.FOREST_FRAC) + df_snowcanopy.THROUGHFALL_rain + df_snowcanopy.SNOW_RUNOFF;
        df_snow.INPUT_snow += df_snowcanopy.MASS_Release;
        if (df_snow.W == 0.0 && df_snow.INPUT_snow > 0.0) {
            /**
             * no snow on the surface, while there is a snowfall coming
            */
           df_snow.Wice = df_snow.INPUT_snow; 
           if (df_snow.INPUT_rain <= df_snow.Wice / 0.94 * 0.06) {
            /***
             * Wliq <= 0.06W; liquid water holding capacity of snowpack
             * Wice >= 0.94W
             * */ 
                df_snow.Wliq = df_snow.INPUT_rain;
                df_snow.SNOW_RUNOFF = 0.0;
           } else {
                df_snow.Wliq = df_snow.Wice / 0.94 * 0.06;
                df_snow.SNOW_RUNOFF = df_snow.INPUT_rain - df_snow.Wice / 0.94 * 0.06;
           }
           df_snow.W = df_snow.Wice + df_snow.Wliq;  
           df_snow.SNOW_TEM = (TS_Meteo+i)->TEM_AIR_AVG;
           // here, density of newly fallen (fresh) snow, kg/m3
           df_snow.SNOW_DENSITY = 67.92 + 51.25 * exp((TS_Meteo+i)->TEM_AIR_AVG / 2.59);  
           df_snow.SNOW_ALBEDO = 0.9; // the albedo of freshly fallen snow
           df_snow.SNOW_DEPTH = SNOW_depth(df_snow.SNOW_DENSITY, df_snow.W);
           df_snow.SNOW_Ras = 0.01; 
           /* energy flux of snowpack */
           FLUX_zero(&df_flux);
        } 
        else if (df_snow.W > 0.0)
        {
            /* snowpack: energy flux */
            df_snow.SNOW_Ras = Resistance_AirSnow(
                // aerodynamic resistance between snow surface and near-surface reference height, [h/m]
                (TS_Meteo+i)->WINSD, p_gp->WIND_HEIGHT, df_snow.SNOW_DEPTH
            );  
            if ((TS_Meteo+i)->TEM_AIR_AVG > 0.0) {  //df_snow.Wliq > 0.0// df_snow.SNOW_TEM
                // melting season, update the Ras, considering the atmospheric stability
                df_snow.SNOW_Ras = AerodynamicResistance(
                    df_snow.SNOW_Ras,
                    RichardsonNumber(
                        (TS_Meteo+i)->TEM_AIR_AVG,
                        df_snow.SNOW_TEM,
                        (TS_Meteo+i)->WINSD, 
                        p_gp->WIND_HEIGHT
                    )
                );
            }
            
            df_flux.net_radiation = FLUX_net_radiation(
                (TS_Meteo + i)->R_LONG * 1000 / p_gp->STEP, 
                (TS_Meteo + i)->R_SHORT * 1000/p_gp->STEP * (1-df_surface.ALBEDO_CANOPY)*exp(-df_surface.LAI) * df_surface.FOREST_FRAC + (1-df_surface.FOREST_FRAC), 
                df_snow.SNOW_TEM, 
                df_snow.SNOW_ALBEDO);
            df_flux.sensible = FLUX_sensible(
                (TS_Meteo + i)->TEM_AIR_AVG,
                df_snow.SNOW_TEM,
                df_snow.SNOW_Ras);
            df_flux.latent = FLUX_latent(
                (TS_Meteo+i)->TEM_AIR_AVG,
                df_snow.SNOW_TEM,
                (TS_Meteo+i)->AIRPRE,
                (TS_Meteo+i)->RHU,
                df_snow.SNOW_Ras,
                (df_snow.Wliq > 0.0)? 1:0  // whether liquid water exists in the snowpack, yes: 1
            );
            df_flux.advect = FLUX_advect(
                (TS_Meteo+i)->TEM_AIR_AVG,
                df_snow.INPUT_rain,
                df_snow.INPUT_snow,
                p_gp->STEP
            );

            /* snowpack: mass balance */
            SnowMassBalance(
                df_flux.net_radiation,
                df_flux.sensible,
                df_flux.latent,
                df_flux.advect,
                &df_snow.SNOW_TEM,
                &df_snow.Wliq,
                &df_snow.Wice,
                &df_snow.W,
                df_snow.INPUT_rain,
                df_snow.INPUT_snow,
                p_gp->STEP,
                &df_snow.SNOW_RUNOFF,
                &df_snow.MASS_Release,     // double *MASS_release, // mass release from snowpack (canopy)
                &df_snow.THROUGHFALL_rain, // double *THROUGHFALL_rain
                df_surface.Canopy_snow_c,        // double Canopy_snow_c;    // maximum interception capacity of canopy snow, [m]
                df_surface.LAI2,                 // double LAI2, // all-sided leaf-area-index
                1                                // int G_or_C   // indicator, 1: ground snow; 0: canopy snow
            );
            
            /* update the snow property: density, albedo, depth */
            SnowDensity(
                &(df_snow.SNOW_DENSITY),
                df_snow.SNOW_TEM,
                (df_snow.Wliq > 0.0)?1:0, // Density_BulkWater, whether there is liquid water in snowpack
                df_snow.INPUT_snow, // unit: m 
                df_snow.W,
                p_gp->STEP
            );
            df_snow.SNOW_DEPTH = SNOW_depth(df_snow.SNOW_DENSITY, df_snow.W);
            SnowAlbedo(
                &df_snow.SNOW_ALBEDO,
                (TS_Meteo+i)->SNOWFALL_Interval,
                ((df_snow.Wliq > 0.0)?0:1) // 1: snow accumulation season; 0: snow melting season
            );
            // printf("%6.1f\t%6.2f\t%6.3f\n",(TS_Meteo+i)->WINSD, df_snow.SNOW_DEPTH, df_snow.SNOW_Ras);
            
        } else {
            /******
                df_snow.W == 0.0 && df_snow.INPUT_snow == 0.0, 
                nothing related to snow process happens
            */
           Initialize_snow(&df_snow);
           df_snow.SNOW_RUNOFF = df_snow.INPUT_rain;
           
           FLUX_zero(&df_flux); // zero energy flux on snowpack
        }
            
        /* output the snow results */
        fprintf(
            f_output,
            "%d,%d,%d,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f\n", 
            (TS_Meteo+i)->date.y, (TS_Meteo+i)->date.m, (TS_Meteo+i)->date.d, 
            df_snow.SNOW_DEPTH, df_snow.W, df_snow.Wice, df_snow.Wliq, df_snow.SNOW_RUNOFF, 
            df_snow.SNOW_ALBEDO, df_snow.SNOW_DENSITY, df_snow.SNOW_Ras, df_snow.SNOW_TEM
        ); 
        
        fprintf(
            f_output_flux,
            "%d,%d,%d,%6.2f,%6.2f,%6.2f,%6.2f\n",
            (TS_Meteo+i)->date.y, (TS_Meteo+i)->date.m, (TS_Meteo+i)->date.d, 
            df_flux.net_radiation, df_flux.sensible, df_flux.latent, df_flux.advect
            );
    }
    fclose(f_output);
    printf("---- Computation done! -------");
    return 0;
};

void import_global(
    char fname[], 
    Struct_Para_global *p_gp,
    Struct_surface *p_surface
){
    /**************
     * import the global parameters into memory
     * 
     * -- Parameters:
     *      fname: a string (1-D character array), file path and name of the global parameters
     * -- Output:
     *      return a structure containing the key fields
     * ********************/
    
    char row[MAXCHAR];
    FILE *fp;
    char *token;
    char *token2;
    int i;
    int VAR_i=0;
    if ((fp=fopen(fname, "r")) == NULL) {
        printf("cannot open global parameter file\n");
        exit(0);
    }
    while (!feof(fp))
    {
        // the fgets() function comes from <stdbool.h>
        // Reads characters from stream and stores them as a C string
        fgets(row, MAXCHAR, fp); 
        if (row != NULL && strlen(row) > 1) {
            /*non-empty row(string)*/
            if (row[0] != '#') {
                /* the first character of row should not be # */
                for (i=0;i<strlen(row);i++) {
                    /* remove all the characters after # */
                    if (row[i] == '#') {
                        row[i] = '\0';  // replace the '#' with '\0', end sign.
                    }
                }
                // printf("it is a valid row!\n");
                // printf("Row: %s", row);
                /*assign the values to the parameter structure: key-value pairs*/
                token = strtok(row, ",");   // the first column: key
                token2 = strtok(NULL, ",\n");  // the second column: value
                // printf("token: %s\n", token);
                if (strcmp(token, "LAT") == 0) {
                    p_surface->LAT = atof(token2);
                } else if (strcmp(token, "LAI") == 0) {
                    p_surface->LAI = atof(token2);
                } else if (strcmp(token, "ALBEDO_CANOPY") == 0) {
                    p_surface->ALBEDO_CANOPY = atof(token2);
                } else if (strcmp(token, "FOREST_FRAC") == 0) {
                    p_surface->FOREST_FRAC = atof(token2);
                } else if (strcmp(token, "START_YEAR") == 0) {
                    p_gp->START_YEAR = atoi(token2);
                } else if (strcmp(token, "START_MONTH") == 0) {
                    p_gp->START_MONTH = atoi(token2);
                } else if (strcmp(token, "START_DAY") == 0) {
                    p_gp->START_DAY = atoi(token2);
                } else if (strcmp(token, "STEP") == 0) {
                    p_gp->STEP = atoi(token2);
                } else if (strcmp(token, "FILE_PATH") == 0) {
                    strcpy(p_gp->FILE_PATH, token2);
                } else if (strcmp(token, "FILE_OUT") == 0) {
                    strcpy(p_gp->FILE_OUT, token2);
                } else if (strcmp(token, "FILE_OUT_flux") == 0) {
                    strcpy(p_gp->FILE_OUT_flux, token2);
                } else if (strcmp(token, "NA") == 0) {
                    p_gp->NA = atof(token2);
                } else if (strcmp(token, "WIND_HEIGHT") == 0) {
                    p_gp->WIND_HEIGHT = atof(token2);
                } else if (strcmp(token, "N_COL") == 0) {
                    p_gp->N_COL = atoi(token2);   
                } else if (strcmp(token, "VAR") == 0) {
                    strcpy(p_gp->VAR[VAR_i++], token2);
                } else {
                    printf(
                        "Error in opening global parameter file: unrecognized parameter field!"
                    );
                    exit(0);
                }
            }
        }
    }
    fclose(fp);
}

int import_Meteo(
    Struct_Para_global *p_gp,
    Struct_Meteo *P_meteo
){
    FILE *fp_meteo;
    if ((fp_meteo=fopen(p_gp->FILE_PATH, "r")) == NULL) {
        printf("Cannot open hourly rr data file!\n");
        exit(0);
    }

    char *token;
    char row[MAXCHAR];
    int i, nrow;
    // int N_STATION = 134;  // key parameter

    i = 0;
    while (!feof(fp_meteo)) {
        fgets(row, MAXCHAR, fp_meteo);
        (P_meteo + i)->date.y = atoi(strtok(row, ","));
        (P_meteo + i)->date.m = atoi(strtok(NULL, ","));
        (P_meteo + i)->date.d = atoi(strtok(NULL, ","));
        (P_meteo + i)->PREC = atof(strtok(NULL, ",")); 
        (P_meteo + i)->WINSD = atof(strtok(NULL, ",")); 
        (P_meteo + i)->SUNDUR = atof(strtok(NULL, ","));
        (P_meteo + i)->AIRPRE = atof(strtok(NULL, ","));
        (P_meteo + i)->TEM_AIR_AVG = atof(strtok(NULL, ","));
        (P_meteo + i)->RHU = atof(strtok(NULL, ","));
        (P_meteo + i)->R_LONG = atof(strtok(NULL, ","));
        (P_meteo + i)->R_SHORT = atof(strtok(NULL, ","));
        i = i + 1;
    }
    nrow = i-1;
    fclose(fp_meteo);
    return nrow;
}

void FLUX_zero(
    Struct_snow_flux *p_df_flux
){
    p_df_flux->advect = 0.0;
    p_df_flux->Heat_PhaseChange = 0.0;
    p_df_flux->latent = 0.0;
    p_df_flux->net_radiation = 0.0;
    p_df_flux->sensible = 0.0;
}

void Initialize_snow(
    Struct_Snow *p_df_snow)
{
    p_df_snow->INPUT_rain = 0.0;
    p_df_snow->INPUT_snow = 0.0;
    p_df_snow->MASS_Release = 0.0;
    p_df_snow->SNOW_ALBEDO = 0.0;
    p_df_snow->SNOW_DENSITY = 0.0;
    p_df_snow->SNOW_DEPTH = 0.0;
    p_df_snow->SNOW_Ras = 0.0;
    p_df_snow->SNOW_RUNOFF = 0.0;
    p_df_snow->SNOW_TEM = 0.0;
    p_df_snow->THROUGHFALL_rain = 0.0;
    p_df_snow->W = 0.0;
    p_df_snow->Wice = 0.0;
    p_df_snow->Wliq = 0.0;
}