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
    double W; // snow water equivalent
    double Wice;
    double Wliq;
    double SNOW_ALBEDO;
    double SNOW_RUNOFF;
    double SNOW_TEM;
    double SNOW_DENSITY;
    double SNOW_Ras; //aerodynamic resistance
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
    double LAI;
    double ALBEDO_CANOPY;
    double FOREST_FRAC;
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
        -99.9,
        7,
        {"SKIP", "SKIP", "SKIP", "WINSD1", "TEM_AIR_AVG1", "RHU1", "SUNDUR1"}
    };
    
    Struct_Para_global *p_gp;  // pointer to struct GlobalPara
    p_gp = &GlobalPara;     
    
    Struct_surface df_surface = {
        2596,
        40.27, 0.0, 0, 0.0
    };

    /******* import the global parameters ***********
    parameter from main() function, pointer array
    argv[0]: pointing to the first string from command line (the executable file)
    argv[1]: pointing to the second string (parameter): file path and name of global parameter file.
    */
    import_global(*(++argv), p_gp, &df_surface);
    
    printf("---- import global parameters ------- \n");
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
            0.0,// SNOWDEPTH;
            0.0,// W; // snow water equivalent
            0.0,// Wice;
            0.0,// Wliq;
            0.0,// SNOW_ALBEDO;
            0.0,// SNOW_RUNOFF;
            0.0,// SNOW_TEM;
            0.0, // SNOW_DENSITY;
            0.0  // aerodynamic resistance
    };
    Struct_snow_flux df_flux = {
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

    for (i = 0; i < nrow; i++)
    {
        if (df_snow.W == 0.0 && (TS_Meteo+i)->SNOWFALL > 0.0) {
            /**
             * no snow on the surface, while there is a snowfall
            */
           df_snow.W = (TS_Meteo+i)->SNOWFALL / 1000;  // unit: mm -> m
           df_snow.Wice = (TS_Meteo+i)->SNOWFALL / 1000;
           df_snow.SNOW_TEM = (TS_Meteo+i)->TEM_AIR_AVG;
           df_snow.SNOW_DENSITY = 120.0;  // density of new snow, kg/m3
           df_snow.SNOW_ALBEDO = 0.9; // the albedo of freshly fallen snow
           df_snow.SNOW_DEPTH = Density_ice / df_snow.SNOW_DENSITY * df_snow.W;
           df_snow.Wliq = 0.0;
           df_snow.SNOW_RUNOFF = 0.0;
           df_snow.SNOW_Ras = 0.0; 
        } 
        else if (df_snow.W > 0.0 && (TS_Meteo+i)->SNOWFALL >= 0.0)
        {
            /* snowpack: energy flux */
            df_flux.net_radiation = FLUX_net_radiation(
                (TS_Meteo+i)->R_LONG * 1000/24, (TS_Meteo+i)->R_SHORT * 1000/24, df_snow.SNOW_TEM, df_snow.SNOW_ALBEDO
            );
            df_snow.SNOW_Ras = Resistance_AirSnow(
                (TS_Meteo+i)->WINSD, 10.0, df_snow.SNOW_DEPTH
            );
            // if ((TS_Meteo+i)->TEM_AIR_AVG > df_snow.SNOW_TEM) {  //df_snow.Wliq > 0.0
            //     // melting season, update the Ras, considering the atmospheric stability
            //     df_snow.SNOW_Ras = AerodynamicResistance(
            //         RichardsonNumber(
            //             (TS_Meteo+i)->TEM_AIR_AVG,
            //             df_snow.SNOW_TEM,
            //             (TS_Meteo+i)->WINSD, 10.0
            //         ),
            //         df_snow.SNOW_Ras
            //     );
            // }
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
                &df_snow.SNOW_DENSITY,
                df_snow.SNOW_TEM,
                (df_snow.Wliq > 0.0)?1:0, // Density_BulkWater, whether there is liquid water in snowpack
                (TS_Meteo+i)->SNOWFALL / 1000, // unit: m 
                df_snow.W,
                p_gp->STEP
            );
            df_snow.SNOW_DEPTH = Density_ice / df_snow.SNOW_DENSITY * df_snow.W;
            SnowAlbedo(
                &df_snow.SNOW_ALBEDO,
                (TS_Meteo+i)->SNOWFALL_Interval,
                ((df_snow.Wliq > 0.0)?0:1) // 1: snow accumulation season; 0: snow melting season
            );
            printf("%6.1f\t%6.2f\t%6.3f\n",(TS_Meteo+i)->WINSD, df_snow.SNOW_DEPTH, df_snow.SNOW_Ras);
            printf(
                "%6.2f\t%6.2f\t%6.2f\t%6.2f\n",
                df_flux.net_radiation, df_flux.sensible, df_flux.latent, df_flux.advect
            );
        } else {
            // df_snow = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
        }
        /*
        df_snow.W == 0.0 && (TS_Meteo+i)->SNOWFALL == 0.0, nothing happens to snow process
        */
    }
    
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

