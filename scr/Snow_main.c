#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "Snow.h"

#define MAXCHAR 3000
#define MAXrow 20000  // almost 100 years long ts

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
    double WINSD;
    double TEM_AIR_AVG;
    double RHU;
    double SUNDUR;
    double AIRPRE;
    double R_SHORT;
    double R_LONG;
} Struct_Meteo;


typedef struct 
{
    struct Date date;  
    double SNOWDEPTH;
    double SWE;  // snow water equivalent
    double W;
    double Wice;
    double Wliq;
    double SNOW_ALBEDO;
    double SNOW_RUNOFF;
    double SNOW_TEM;
    double SNOW_DENSITY;
} Struct_Snow;

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
    int nrow = 0; 
    nrow = import_Meteo(p_gp, TS_Meteo);
    printf("number of rows: %d\n", nrow);

    int i;
    for (i = 0; i < nrow; i++) 
    {
        
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

