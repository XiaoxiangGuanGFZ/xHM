
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "Radiation_Calc.h"
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

    double LAT;
    double LAI;
    double ALBEDO_CANOPY;
    double ALBEDO_SNOW;
    double FOREST_FRAC;

    char FILE_PATH[200];
    char FILE_OUT_RADIA[200];
    double NA;
    int N_COL;
    char VAR[7][20];
} Struct_Para_global;

typedef struct Struct_Meteo
{
    struct Date date;    
    double WINSD;
    double TEM_AIR_AVG;
    double RHU;
    double SUNDUR;

    double R_sky;
    double R_short;
    double R_long;
} Struct_Meteo;


/* functions declaration */
void import_global(char fname[], Struct_Para_global *p_gp);  // function declaration
int import_Meteo(
    Struct_Para_global *p_gp,
    Struct_Meteo *P_meteo
);
void Write_radiation(
    Struct_Para_global *p_gp,
    Struct_Meteo *p_meteo,
    int nrow
);

int main(int argc, char * argv[]) {
    /*
    int argc: the number of parameters of main() function;
    char *argv[]: pointer array
    */
    /* char fname[100] = "D:/xHM/example_data/Radiation_Calc_Global_paras.txt";
        this should be the only extern input for this program */

    Struct_Para_global GlobalPara = {
        1990,1,1,24,
        32.10,
        0.0,
        0.0,
        0.0,
        0.0,
        "example.txt",
        "example_output.txt",
        -99.9,
        7,
        {"SKIP", "SKIP", "SKIP", "WINSD1", "TEM_AIR_AVG1", "RHU1", "SUNDUR1"}
    };
    
    Struct_Para_global *p_gp;  // pointer to struct GlobalPara
    p_gp = &GlobalPara;     
    
    /******* import the global parameters ***********
    parameter from main() function, pointer array
    argv[0]: pointing to the first string from command line (the executable file)
    argv[1]: pointing to the second string (parameter): file path and name of global parameter file.
    */
    import_global(*(++argv), p_gp);
    
    printf("---- import global parameters ------- \n");
    printf("START_YEAR: %d\nSTART_MONTH: %d\nSTART_DAY: %d\nSTEP: %d\n",
        p_gp->START_YEAR, p_gp->START_MONTH, p_gp->START_DAY, p_gp->STEP
        );
    printf("LAT: %.2f\nLAI: %.2f\nALBEDO_CANOPY: %.2f\nALBEDO_SNOW: %.2f\nFOREST_FRAC: %.2f\n",
        p_gp->LAT, p_gp->LAI, p_gp->ALBEDO_CANOPY, p_gp->ALBEDO_SNOW, p_gp->FOREST_FRAC
        );
    printf(
        "FILE_PATH: %s\nFILE_OUT_RADIA: %s\n",
        p_gp->FILE_PATH, p_gp->FILE_OUT_RADIA
    );

    Struct_Meteo TS_Meteo[MAXrow];
    int nrow = 0; 
    nrow = import_Meteo(p_gp, TS_Meteo);
    printf("number of rows: %d\n", nrow);

    int i;
    for (i = 0; i < nrow; i++) 
    {
        if (
            (TS_Meteo+i)->SUNDUR == p_gp->NA ||
            (TS_Meteo+i)->RHU == p_gp->NA ||
            (TS_Meteo+i)->TEM_AIR_AVG == p_gp->NA ||
            (TS_Meteo+i)->WINSD == p_gp->NA
        )
        {
            (TS_Meteo+i)->R_sky = p_gp->NA;
            (TS_Meteo+i)->R_short = p_gp->NA;
            (TS_Meteo+i)->R_long = p_gp->NA;
        } else {
            (TS_Meteo+i)->R_sky = Radiation_downward_short(
                (TS_Meteo+i)->date.y,
                (TS_Meteo+i)->date.m,
                (TS_Meteo+i)->date.d,
                p_gp->LAT,  // the latitute of the location
                (TS_Meteo+i)->SUNDUR,   // sunshine duration in a day, hours
                0.25, 0.5
            );
            (TS_Meteo+i)->R_short = Radiation_short_surface(
                (TS_Meteo+i)->R_sky, p_gp->ALBEDO_CANOPY,  p_gp->LAI, p_gp->ALBEDO_SNOW 
            );
            (TS_Meteo+i)->R_long = Radiation_long_surface(
                (TS_Meteo+i)->TEM_AIR_AVG, (TS_Meteo+i)->RHU, p_gp->FOREST_FRAC
            );
        }
    }

    // for (i = 0; i < nrow; i++)
    // {
    //     printf("%d-%d-%d\t%6.2f\t%6.2f\t%6.2f\n", 
    //         (TS_Meteo+i)->date.y, (TS_Meteo+i)->date.m, (TS_Meteo+i)->date.d,
    //         (TS_Meteo+i)->R_sky, (TS_Meteo+i)->R_short, (TS_Meteo+i)->R_long);
    // }
    Write_radiation(
        p_gp, TS_Meteo, nrow
    );
    printf("---- Computation done! -------");
    return 0;
};

void import_global(
    char fname[], Struct_Para_global *p_gp
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
                    p_gp->LAT = atof(token2);
                } else if (strcmp(token, "LAI") == 0) {
                    p_gp->LAI = atof(token2);
                } else if (strcmp(token, "ALBEDO_CANOPY") == 0) {
                    p_gp->ALBEDO_CANOPY = atof(token2);
                } else if (strcmp(token, "ALBEDO_SNOW") == 0) {
                    p_gp->ALBEDO_SNOW = atof(token2);
                } else if (strcmp(token, "FOREST_FRAC") == 0) {
                    p_gp->FOREST_FRAC = atof(token2);
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
                } else if (strcmp(token, "FILE_OUT_RADIA") == 0) {
                    strcpy(p_gp->FILE_OUT_RADIA, token2);
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

        (P_meteo + i)->WINSD = atof(strtok(NULL, ",")); 
        (P_meteo + i)->TEM_AIR_AVG = atof(strtok(NULL, ","));
        (P_meteo + i)->RHU = atof(strtok(NULL, ","));
        (P_meteo + i)->SUNDUR = atof(strtok(NULL, ","));
        
        i = i + 1;
    }
    nrow = i-1;
    fclose(fp_meteo);
    return nrow;
}


void Write_radiation(
    Struct_Para_global *p_gp,
    Struct_Meteo *p_meteo,
    int nrow
){
    /**************
     * Description:
     *      write the radiation output into output file (.csv)
     * Parameters:
     *      p_gp: global parameters 
     *      p_FP_OUT: a FILE pointer, pointing to the output file
     * 
     * ************/
    int i;
    FILE *p_FP_OUT;
    if ((p_FP_OUT=fopen(p_gp->FILE_OUT_RADIA, "w")) == NULL) {
        printf("Program terminated: cannot create or open output file\n");
        exit(0);
    }

    for (i = 0; i < nrow; i++) {
        fprintf(
            p_FP_OUT,
            "%d,%d,%d,%.2f,%.2f", 
            (p_meteo+i)->date.y, (p_meteo+i)->date.m, (p_meteo+i)->date.d, 
            (p_meteo+i)->R_short, (p_meteo+i)->R_long
        ); 
        fprintf(p_FP_OUT, "\n"); // print "\n" after one row
    }
}


