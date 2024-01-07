
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>

#include "GEO_ST.h"
#include "Route_UH.h"

void Import_para(
    char FP[],
    char FP_GEO[],
    char FP_UH[],
    int *step_time,
    double *Velocity_avg,
    double *Velocity_max,
    double *Velocity_min,
    double *b,
    double *c
);

void main(int argc, char * argv[])
{
    char FP_GEO[MAXCHAR];
    char FP_UH[MAXCHAR];
    int step_time;
    double Velocity_avg, Velocity_max, Velocity_min;
    double b, c;

    Import_para(
        *(++argv), FP_GEO, FP_UH,
        &step_time,
        &Velocity_avg,
        &Velocity_max,
        &Velocity_min,
        &b,
        &c);

    UH_Generation(
        FP_GEO,
        FP_UH,
        step_time,
        Velocity_avg,
        Velocity_max,
        Velocity_min,
        b,
        c);
}

void Import_para(
    char FP[],
    char FP_GEO[],
    char FP_UH[],
    int *step_time,
    double *Velocity_avg,
    double *Velocity_max,
    double *Velocity_min,
    double *b,
    double *c
)
{
    char row[MAXCHAR];
    char S1[MAXCHAR];
    char S2[MAXCHAR];

    FILE *fp;
    if ((fp = fopen(FP, "r")) == NULL)
    {
        printf("cannot open global parameter file %s\n", FP);
        exit(0);
    }
    int i;
    while (fgets(row, MAXCHAR, fp) != NULL)
    {
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
                
                if (strcmp(S1, "FP_GEO") == 0)
                {
                    strcpy(FP_GEO, S2);
                }
                else if (strcmp(S1, "FP_UH") == 0)
                {
                    strcpy(FP_UH, S2);
                }
                else if (strcmp(S1, "step_time") == 0)
                {
                    *step_time = atoi(S2);
                }
                else if (strcmp(S1, "Velocity_avg") == 0)
                {
                    *Velocity_avg = atof(S2);
                }
                else if (strcmp(S1, "Velocity_max") == 0)
                {
                    *Velocity_max = atof(S2);
                }
                else if (strcmp(S1, "Velocity_min") == 0)
                {
                    *Velocity_min = atof(S2);
                }
                else if (strcmp(S1, "b") == 0)
                {
                    *b = atof(S2);
                }
                else if (strcmp(S1, "c") == 0)
                {
                    *c = atof(S2);
                }
                else
                {
                    printf("Unrecognized field in %s!", FP);
                    exit(0);
                }
            }
        }
    }
}

