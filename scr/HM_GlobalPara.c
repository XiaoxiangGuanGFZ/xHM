/*
 * SUMMARY:      HM_GlobalPara.c
 * USAGE:        read and initialize the global parameter structure 
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Jan-2024
 * DESCRIPTION:  handle (read and check) global parameter file
 * DESCRIP-END.
 * FUNCTIONS:    Import_GlobalPara(); Initialize_GlobalPara();
 * 
 * COMMENTS:
 * 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Constants.h"
#include "HM_ST.h"
#include "HM_GlobalPara.h"

void Import_GlobalPara(
    char FP[],
    GLOBAL_PARA *global_para
)
{
    FILE *fp;
    if ((fp = fopen(FP, "r")) == NULL)
    {
        printf("cannot open file %s\n", FP);
        exit(0);
    }
    int i;  // character indexing in a string,
    int j = 0;
    char row[MAXCHAR];
    char S1[MAXCHAR];
    char S2[MAXCHAR];
    while (fgets(row, MAXCHAR, fp) != NULL)
    {
        j++;
        if (row != NULL && strlen(row) > 1 && row[0] != '#')
        {
            for (i = 0; i < strlen(row); i++)
            {
                /* remove all the characters after # */
                if (row[i] == '#')
                {
                    row[i] = '\0'; // replace the '#' with '\0', end sign.
                }
            }
            if (sscanf(row, "%[^,],%s", S1, S2) == 2)
            {
                if (strcmp(S1, "FP_PRE") == 0)
                {
                    strcpy(global_para->FP_PRE, S2);
                }
                else if (strcmp(S1, "FP_PRS") == 0)
                {
                    strcpy(global_para->FP_PRS, S2);
                }
                else if (strcmp(S1, "FP_WIN") == 0)
                {
                    strcpy(global_para->FP_WIN, S2);
                } 
                else if (strcmp(S1, "FP_SSD") == 0)
                {
                    strcpy(global_para->FP_SSD, S2);
                }
                else if (strcmp(S1, "FP_TEM_AVG") == 0)
                {
                    strcpy(global_para->FP_TEM_AVG, S2);
                }
                else if (strcmp(S1, "FP_TEM_MAX") == 0)
                {
                    strcpy(global_para->FP_TEM_MAX, S2);
                }
                else if (strcmp(S1, "FP_TEM_MIN") == 0)
                {
                    strcpy(global_para->FP_TEM_MIN, S2);
                }
                else if (strcmp(S1, "FP_RHU") == 0)
                {
                    strcpy(global_para->FP_RHU, S2);
                }
                else if (strcmp(S1, "FP_GEO") == 0)
                {
                    strcpy(global_para->FP_GEO, S2);
                }
                else if (strcmp(S1, "FP_OUTNAMELIST") == 0)
                {
                    strcpy(global_para->FP_OUTNAMELIST, S2);
                }
                else if (strcmp(S1, "FP_SOIL_HWSD_ID") == 0)
                {
                    strcpy(global_para->FP_SOIL_HWSD_ID, S2);
                }
                else if (strcmp(S1, "FP_SOILLIB") == 0)
                {
                    strcpy(global_para->FP_SOILLIB, S2);
                } 
                else if (strcmp(S1, "FP_VEGLIB") == 0)
                {
                    strcpy(global_para->FP_VEGLIB, S2);
                } 
                else if (strcmp(S1, "PATH_OUT") == 0)
                {
                    strcpy(global_para->PATH_OUT, S2);
                }
                else if (strcmp(S1, "START_YEAR") == 0)
                {
                    global_para->START_YEAR = atoi(S2);
                }
                else if (strcmp(S1, "START_MONTH") == 0)
                {
                    global_para->START_MONTH = atoi(S2);
                }
                else if (strcmp(S1, "START_DAY") == 0)
                {
                    global_para->START_DAY = atoi(S2);
                }
                else if (strcmp(S1, "START_HOUR") == 0)
                {
                    global_para->START_HOUR = atoi(S2);
                }
                else if (strcmp(S1, "END_YEAR") == 0)
                {
                    global_para->END_YEAR = atoi(S2);
                }
                else if (strcmp(S1, "END_MONTH") == 0)
                {
                    global_para->END_MONTH = atoi(S2);
                }
                else if (strcmp(S1, "END_DAY") == 0)
                {
                    global_para->END_DAY = atoi(S2);
                }
                else if (strcmp(S1, "END_HOUR") == 0)
                {
                    global_para->END_HOUR = atoi(S2);
                }
                else if (strcmp(S1, "STEP_TIME") == 0)
                {
                    global_para->STEP_TIME = atoi(S2);
                }
                else if (strcmp(S1, "SURFACE_RUNOFF") == 0)
                {
                    strcpy(global_para->SURFACE_RUNOFF, S2);
                }
                else if (strcmp(S1, "FP_UH") == 0)
                {
                    strcpy(global_para->FP_UH, S2);
                }
                else if (strcmp(S1, "SOIL_D") == 0)
                {
                    global_para->SOIL_D = atof(S2);
                }
                else if (strcmp(S1, "SOIL_d1") == 0)
                {
                    global_para->SOIL_d1 = atof(S2);
                }
                else if (strcmp(S1, "SOIL_d2") == 0)
                {
                    global_para->SOIL_d2 = atof(S2);
                }
                else if (strcmp(S1, "STREAM_D") == 0)
                {
                    global_para->STREAM_D = atof(S2);
                }
                else if (strcmp(S1, "STREAM_W") == 0)
                {
                    global_para->STREAM_W = atof(S2);
                }
                else if (strcmp(S1, "ROUTE_CHANNEL_k") == 0)
                {
                    global_para->ROUTE_CHANNEL_k = atof(S2);
                }
                else if (strcmp(S1, "WIN_H") == 0)
                {
                    global_para->WIN_H = atof(S2);
                }
                else if (strcmp(S1, "Velocity_avg") == 0)
                {
                    global_para->Velocity_avg = atof(S2);
                }
                else if (strcmp(S1, "Velocity_max") == 0)
                {
                    global_para->Velocity_max = atof(S2);
                }
                else if (strcmp(S1, "Velocity_min") == 0)
                {
                    global_para->Velocity_min = atof(S2);
                }
                else if (strcmp(S1, "b") == 0)
                {
                    global_para->b = atof(S2);
                }
                else if (strcmp(S1, "c") == 0)
                {
                    global_para->c = atof(S2);
                }
                else
                {
                    printf("Unrecognized field in row %d: %s\n", j, row);
                    exit(0);
                }
            }
        }
    }
    fclose(fp);
}



void Initialize_GlobalPara(
    GLOBAL_PARA *global_para
)
{
    /* weather data parameters */
    strcpy(global_para->FP_PRE, "\0");
    strcpy(global_para->FP_PRS, "\0");
    strcpy(global_para->FP_RHU, "\0");
    strcpy(global_para->FP_SSD, "\0");
    strcpy(global_para->FP_WIN, "\0");
    strcpy(global_para->FP_TEM_AVG, "\0");
    strcpy(global_para->FP_TEM_MAX, "\0");
    strcpy(global_para->FP_TEM_MIN, "\0");

    /* GEO info parameters */
    strcpy(global_para->FP_SOIL_HWSD_ID, "\0");
    strcpy(global_para->FP_SOILLIB, "\0");
    strcpy(global_para->FP_GEO, "\0");
    strcpy(global_para->FP_VEGLIB, "\0");

    /* model setup parameters */
    global_para->START_DAY = 1; global_para->END_DAY = 1;
    global_para->START_HOUR = 0; global_para->END_HOUR = 0;
    global_para->START_MONTH = 1; global_para->END_MONTH = 1;
    global_para->START_YEAR = 1900; global_para->END_YEAR = 1900;
    global_para->STEP_TIME = 24;
    
    /* hydrological modeling parameters */
    global_para->ROUTE_CHANNEL_k = 0.3;
    global_para->WIN_H = 10.0;
    global_para->SOIL_d1 = 0.1;
    global_para->SOIL_d2 = 0.2;
    global_para->SOIL_D = 1.5;
    global_para->STREAM_D = 1;
    global_para->STREAM_W = 10;

    /* UH parameters */
    strcpy(global_para->SURFACE_RUNOFF, "UH");
    strcpy(global_para->FP_UH, "\0");
    global_para->Velocity_avg = 480.0;
    global_para->Velocity_max = 13200.0;
    global_para->Velocity_min = 100.0;
    global_para->b = 0.5; global_para->c = 0.5;

    /* output parameters */
    strcpy(global_para->FP_OUTNAMELIST, "\0");
    strcpy(global_para->PATH_OUT, "\0");
}

void Print_GlobalPara(
    GLOBAL_PARA *gp
)
{
    printf("%19s %s\n", "FIELD", "VALUE");
    printf("%19s %s\n", "***************", "***************");
    printf("%18s: %d\n", "START_YEAR", gp->START_YEAR);
    printf("%18s: %d\n", "START_MONTH", gp->START_MONTH);
    printf("%18s: %d\n", "START_DAY", gp->START_DAY);
    printf("%18s: %d\n", "START_HOUR", gp->START_HOUR);
    printf("%18s: %d\n", "END_YEAR", gp->END_YEAR);
    printf("%18s: %d\n", "END_MONTH", gp->END_MONTH);
    printf("%18s: %d\n", "END_DAY", gp->END_DAY);
    printf("%18s: %d\n", "END_HOUR", gp->END_HOUR);
    printf("%18s: %d\n", "STEP_TIME", gp->STEP_TIME);

    printf("%18s: %s\n", "FP_PRE", gp->FP_PRE);
    printf("%18s: %s\n", "FP_PRS", gp->FP_PRS);
    printf("%18s: %s\n", "FP_RHU", gp->FP_RHU);
    printf("%18s: %s\n", "FP_SSD", gp->FP_SSD);
    printf("%18s: %s\n", "FP_WIN", gp->FP_WIN);
    printf("%18s: %s\n", "FP_TEM_AVG", gp->FP_TEM_AVG);
    printf("%18s: %s\n", "FP_TEM_MAX", gp->FP_TEM_MAX);
    printf("%18s: %s\n", "FP_TEM_MIN", gp->FP_TEM_MIN);
    printf("%18s: %f\n", "WIN_H", gp->WIN_H);

    printf("%18s: %s\n", "FP_RHU", gp->FP_GEO);
    printf("%18s: %s\n", "FP_VEGLIB", gp->FP_VEGLIB);
    printf("%18s: %s\n", "FP_SOILLIB", gp->FP_SOILLIB);
    printf("%18s: %s\n", "FP_SOIL_HWSD_ID", gp->FP_SOIL_HWSD_ID);
    printf("%18s: %f\n", "SOIL_D", gp->SOIL_D);
    printf("%18s: %f\n", "SOIL_d1", gp->SOIL_d1);
    printf("%18s: %f\n", "SOIL_d2", gp->SOIL_d2);
    printf("%18s: %f\n", "STREAM_D", gp->STREAM_D);
    printf("%18s: %f\n", "STREAM_W", gp->STREAM_W);
    printf("%18s: %f\n", "ROUTE_CHANNEL_k", gp->ROUTE_CHANNEL_k);

    printf("%18s: %s\n", "SURFACE_RUNOFF", gp->SURFACE_RUNOFF);
    printf("%18s: %s\n", "FP_UH", gp->FP_UH);
    printf("%18s: %f\n", "Velocity_avg", gp->Velocity_avg);
    printf("%18s: %f\n", "Velocity_max", gp->Velocity_max);
    printf("%18s: %f\n", "Velocity_min", gp->Velocity_min);
    printf("%18s: %f\n", "b", gp->b);
    printf("%18s: %f\n", "c", gp->c);

    printf("%18s: %s\n", "PATH_OUT", gp->PATH_OUT);
    printf("%18s: %s\n", "FP_OUTNAMELIST", gp->FP_OUTNAMELIST);

    printf("%19s %s\n", "***************", "***************");
}

// int main(int argc, char const *argv[])
// {
//     char FP[MAXCHAR] = "D:/xHM/example_data/Para_ET2.txt";
//     GLOBAL_PARA global_para;

//     Initialize_GlobalPara(&global_para);
//     printf("FP_GEO: %s\n", global_para.FP_GEO);

//     Import_GlobalPara(FP, &global_para);
//     printf("FP_GEO: %s\n", global_para.FP_GEO);
//     return 0;
// }

