
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "Radiation_Calc.h"
#define MAXCHAR 3000
#define MAXrow 30000  // almost 100 years long ts

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

    char FILE_PATH[100];
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
        -99.9,
        7,
        {"SKIP", "SKIP", "SKIP", "WINSD1", "TEM_AIR_AVG1", "RHU1", "SUNDUR1"}
    };
    
    Struct_Para_global *p_gp;  // pointer to struct GlobalPara
    p_gp = &GlobalPara; 
    printf("%s\n%s\n%s\n", p_gp->FILE_PATH, p_gp->VAR[1], p_gp->VAR[3]);
    
    
    /******* import the global parameters ***********
    parameter from main() function, pointer array
    argv[0]: pointing to the first string from command line (the executable file)
    argv[1]: pointing to the second string (parameter): file path and name of global parameter file.
    */
    import_global(*(++argv), p_gp);
    printf("%s\n%s\n%s\n", p_gp->FILE_PATH, p_gp->VAR[0], p_gp->VAR[3]);
    
    Struct_Meteo TS_Meteo[15000];
    int nrow = 0; 
    nrow = import_Meteo(p_gp, TS_Meteo);
    printf("number of rows: %d\n", nrow);

    int i;
    // for (i = 0; i < nrow; i++)
    // {
    //     printf("%d-%d-%d\t%6.2f\t%6.2f\t%6.2f\n", 
    //         (TS_Meteo+i)->date.y, (TS_Meteo+i)->date.m, (TS_Meteo+i)->date.d,
    //         (TS_Meteo+i)->RHU, (TS_Meteo+i)->SUNDUR, (TS_Meteo+i)->WINSD);
    // }

    for (i = 0; i < nrow; i++) 
    {
        if (
            (TS_Meteo+i)->SUNDUR <= p_gp->NA ||
            (TS_Meteo+i)->RHU <= p_gp->NA ||
            (TS_Meteo+i)->TEM_AIR_AVG <= p_gp->NA ||
            (TS_Meteo+i)->WINSD <= p_gp->NA
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

    for (i = 0; i < nrow; i++)
    {
        printf("%d-%d-%d\t%6.2f\t%6.2f\t%6.2f\n", 
            (TS_Meteo+i)->date.y, (TS_Meteo+i)->date.m, (TS_Meteo+i)->date.d,
            (TS_Meteo+i)->R_sky, (TS_Meteo+i)->R_short, (TS_Meteo+i)->R_long);
    }

    // int year = 1999;
    // int month = 1;
    // int day[] = {1,2,3,4,5,6};
    // double lat = 30.5;
    // double Ta[] = {19.5, 20.5, 25.5, 30, 27.4, 20.5};
    // double Tmin[] = {10.5, 10.5, 15.5, 15, 15.4, 10.5};
    // double Tmax[] = {25.5, 26.5, 30.5, 38, 35.4, 29.5};
    // double Wv[] = {1.52, 1.56, 2.23, 1.5, 5.0, 2.36};
    // double RH[] = {50.0, 25.0, 56.0, 78.0, 45.0, 55.0};
    // double Pa[] = {0.894, 0.589, 1.01, 0.958, 0.789, 0.855};
    // double n[] = {9, 10, 8, 8.9, 7.5, 4.5};
    // double J[] = {1, 2, 3, 4, 5, 6};
    // int i = 0;
    // double ET;
    // double R_sky, R_short, R_long;
    // double albedo_forest = 0.13;
    // double LAI = 1.5;
    // double albedo_snow = 0.5;
    // double FF = 0.5;

    // printf("Date \t R_short \t R_long \n");
    // for (i = 0; i < 6; i++) {
    //     R_sky = Radiation_downward_short(
    //         year,
    //         month,
    //         day[i],
    //         lat,  // the latitute of the location
    //         n[i],   // sunshine duration in a day, hours
    //         0.25, 0.5    
    //     );
    //     R_short = Radiation_short_surface(
    //         R_sky, albedo_forest,  LAI, albedo_snow 
    //     );
    //     R_long = Radiation_long_surface(
    //         Ta[i], RH[i], FF
    //     );
        
    //     printf("%d-%d-%d \t", year, month, day[i]);
    //     printf(" %6.3f \t %6.3f \n", R_short, R_long);
    // }

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
                } else if (strcmp(token, "NA") == 0) {
                    p_gp->NA = atof(token2);
                } else if (strcmp(token, "N_COL") == 0) {
                    p_gp->N_COL = atoi(token2);   
                } else if (strcmp(token, "VAR") == 0) {
                    strcpy(p_gp->VAR[VAR_i++], token2);
                    // VAR_i = VAR_i + 1;
                    // p_gp->VAR[VAR_index++] = token2;
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