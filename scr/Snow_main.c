#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "Radiation_Calc.h"
#include "Snow.h"

#define MAXCHAR 3000
#define MAXrow 15000  // almost 100 years long ts

struct Date {
    int y;
    int m;
    int d;
};

typedef struct 
{
    /* data */
    int START_YEAR;
    int START_MONTH;
    int START_DAY;
    int STEP;
    char FILE_PATH[200];
    char FILE_OUT[200];
    char FILE_OUT_flux[200];
    int N_COL;
    double NA;
    char VAR[11][20];
} Struct_Para_global;

typedef struct 
{
    struct Date date;
    double PREC;
    double RAINFALL;
    double SNOWFALL;
    int SNOWFALL_Interval;  //the time since the last snowfall (in days)
    double WINSD;
    double TEM_AIR_AVG;
    double RHU;
    double SUNDUR;
    double AIRPRE;
    double R_SHORT; // unit: MJ/m2 in one day, or MJ/m2/d = 1000/24 (kJ/m2/h)
    double R_LONG;
} Struct_Meteo;


typedef struct 
{
    // struct Date date;  
    double SNOW_DEPTH;
    double W; // snow water equivalent (m)
    double Wice;
    double Wliq;
    double SNOW_ALBEDO;
    double SNOW_RUNOFF;  // excess melting/rainfall water (liquid phase) from snowpack
    double SNOW_TEM;
    double SNOW_DENSITY;
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
    double LAT;
    double LAI;  // single-side Leaf-Area-Index
    double LAI2; // all-side LAI
    double zd;   // zero-plane displacement height (m) of canopy
    double z0;   // roughness height (m)
    double ALBEDO_CANOPY;
    double FOREST_FRAC;   // canopy fraction
    double Canopy_snow_c; // maximum interception capacity of canopy snow, [m]
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
        -99.9,
        7,
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
    printf("START_YEAR: %d\nSTART_MONTH: %d\nSTART_DAY: %d\nSTEP: %d\n",
        p_gp->START_YEAR, p_gp->START_MONTH, p_gp->START_DAY, p_gp->STEP
        );
    printf("LAT: %.2f\nLAI: %.2f\nALBEDO_CANOPY: %.2f\nFOREST_FRAC: %.2f\n",
        df_surface.LAT, df_surface.LAI, df_surface.ALBEDO_CANOPY, df_surface.FOREST_FRAC
        );
    printf(
        "FILE_PATH: %s\nFILE_OUT_RADIA: %s\n",
        p_gp->FILE_PATH, p_gp->FILE_OUT
    );

    Struct_Meteo TS_Meteo[MAXrow];
    int nrow = 0; // number of rows in the data file
    nrow = import_Meteo(p_gp, TS_Meteo);
    printf("number of rows: %d\n", nrow);

    /* check (print) the meteo data */
    int i;
    printf("PREC(mm)\tAir temperature\n");
    for (i = 0; i < 20; i++) 
    {
        printf("%6.2f\t%6.2f\n", (TS_Meteo+i)->PREC, (TS_Meteo+i)->TEM_AIR_AVG);
    }
    printf("---------\n");
    Struct_Snow df_snow = { // initialize 
            0.0, // SNOWDEPTH;
            0.0, // W, snow water equivalent
            0.0, // Wice;
            0.0, // Wliq;
            0.0, // SNOW_ALBEDO;
            0.0, // SNOW_RUNOFF;
            0.0, // SNOW_TEM;
            0.0, // SNOW_DENSITY;
            0.0, // aerodynamic resistance
            0.0  // mass release from snowpack
    };
    Struct_snow_flux df_flux = { // initialize fluxes of snowpack
        0.0, 0.0, 0.0, 0.0, 0.0
    };

    for (i = 0; i < nrow; i++){
        // separate the rainfall and snowfall from precipitation
        Partition_RainSnow(
            &(TS_Meteo+i)->PREC,
            &(TS_Meteo+i)->TEM_AIR_AVG,
            &(TS_Meteo+i)->RAINFALL,
            &(TS_Meteo+i)->SNOWFALL
        );
        if (i == 0) {
            // initialize the SNOWFALL_Interval
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
    for (i = 0; i < nrow; i++)
    {
        if (df_snow.W == 0.0 && (TS_Meteo+i)->SNOWFALL > 0.0) {
            /**
             * no snow on the surface, while there is a snowfall coming
            */
           df_snow.Wice = (TS_Meteo+i)->SNOWFALL / 1000; // unit: mm -> m
           if ((TS_Meteo+i)->RAINFALL / 1000 <= df_snow.Wice / 0.94 * 0.06) {
            /***
             * Wliq <= 0.06W; liquid water holding capacity of snowpack
             * Wice >= 0.94W
             * */ 
                df_snow.Wliq = (TS_Meteo+i)->RAINFALL / 1000;
                df_snow.SNOW_RUNOFF = 0.0;
           } else {
                df_snow.Wliq = df_snow.Wice / 0.94 * 0.06;
                df_snow.SNOW_RUNOFF = (TS_Meteo+i)->RAINFALL / 1000 - df_snow.Wice / 0.94 * 0.06;
           }
           df_snow.W = df_snow.Wice + df_snow.Wliq;  
           df_snow.SNOW_TEM = (TS_Meteo+i)->TEM_AIR_AVG;
           // here, density of newly fallen (fresh) snow, kg/m3
           df_snow.SNOW_DENSITY = 67.92 + 51.25 * exp((TS_Meteo+i)->TEM_AIR_AVG / 2.59);  
           df_snow.SNOW_ALBEDO = 0.9; // the albedo of freshly fallen snow
           df_snow.SNOW_DEPTH = Density_water / df_snow.SNOW_DENSITY * df_snow.W;
           df_snow.SNOW_Ras = 0.01; 
           /* energy flux of snowpack */
           FLUX_zero(&df_flux);
        } 
        else if (df_snow.W > 0.0)
        {
            /* snowpack: energy flux */
            df_snow.SNOW_Ras = Resistance_AirSnow(
                // aerodynamic resistance between snow surface and near-surface reference height, [h/m]
                (TS_Meteo+i)->WINSD, 10.0, df_snow.SNOW_DEPTH
            );  
            if ((TS_Meteo+i)->TEM_AIR_AVG > 0.0) {  //df_snow.Wliq > 0.0// df_snow.SNOW_TEM
                // melting season, update the Ras, considering the atmospheric stability
                df_snow.SNOW_Ras = AerodynamicResistance(
                    df_snow.SNOW_Ras,
                    RichardsonNumber(
                        (TS_Meteo+i)->TEM_AIR_AVG,
                        df_snow.SNOW_TEM,
                        (TS_Meteo+i)->WINSD, 10.0
                    )
                );
            }
            df_flux.net_radiation = FLUX_net_radiation(
                (TS_Meteo+i)->R_LONG * 1000/24, (TS_Meteo+i)->R_SHORT * 1000/24, df_snow.SNOW_TEM, df_snow.SNOW_ALBEDO
            );
            df_flux.sensible = FLUX_sensible(
                (TS_Meteo+i)->TEM_AIR_AVG,
                df_snow.SNOW_TEM,
                df_snow.SNOW_Ras
            );
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
                (TS_Meteo+i)->RAINFALL / 1000, // unit: mm -> m
                (TS_Meteo+i)->SNOWFALL / 1000,
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
                (TS_Meteo+i)->RAINFALL / 1000, // unit: mm -> m
                (TS_Meteo+i)->SNOWFALL / 1000,
                p_gp->STEP,
                &df_snow.SNOW_RUNOFF
            );
            
            /* update the snow property: density, albedo, depth */
            SnowDensity(
                &(df_snow.SNOW_DENSITY),
                df_snow.SNOW_TEM,
                (df_snow.Wliq > 0.0)?1:0, // Density_BulkWater, whether there is liquid water in snowpack
                (TS_Meteo+i)->SNOWFALL / 1000, // unit: m 
                df_snow.W,
                p_gp->STEP
            );
            df_snow.SNOW_DEPTH = Density_water / df_snow.SNOW_DENSITY * df_snow.W;
            SnowAlbedo(
                &df_snow.SNOW_ALBEDO,
                (TS_Meteo+i)->SNOWFALL_Interval,
                ((df_snow.Wliq > 0.0)?0:1) // 1: snow accumulation season; 0: snow melting season
            );
            // printf("%6.1f\t%6.2f\t%6.3f\n",(TS_Meteo+i)->WINSD, df_snow.SNOW_DEPTH, df_snow.SNOW_Ras);
            
        } else {
            /* else: 
            df_snow.W == 0.0 && (TS_Meteo+i)->SNOWFALL == 0.0, nothing happens to snow process
            */
           df_snow.SNOW_RUNOFF = (TS_Meteo+i)->RAINFALL;
           df_snow.W = 0.0; df_snow.Wice = 0.0; df_snow.Wliq = 0.0;
           df_snow.SNOW_DENSITY = 0.0; df_snow.SNOW_DEPTH = 0.0; df_snow.SNOW_TEM = p_gp->NA;
           df_snow.SNOW_Ras = p_gp->NA; df_snow.SNOW_ALBEDO = p_gp->NA;
           
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
