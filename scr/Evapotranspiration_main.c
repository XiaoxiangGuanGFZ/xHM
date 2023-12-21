
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <netcdf.h>
#include "constants.h"
#include "Evapotranspiration_ST.h"
#include "Evapotranspiration.h"
#include "Evapotranspiration_Energy.h"
#include "Evaporation_soil.h"

void Import_GPara(
    char FP_GP[],
    GPara_ET_ST *GP
);

void main(int argc, char * argv[])
{
    GPara_ET_ST GP_ET;
    Import_GPara(*(++argv), &GP_ET);
    printf("%s\n", GP_ET.FP_WEATHER);
}

void Import_GPara(
    char FP_GP[],
    GPara_ET_ST *GP
)
{
    FILE *fp;
    if ((fp = fopen(FP_GP, "r")) == NULL)
    {
        printf("cannot open %s file\n", FP_GP);
        exit(0);
    }
    int i;
    char row[MAXCHAR];
    char S1[MAXCHAR];
    char S2[MAXCHAR];
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
                if (strcmp(S1, "FP_PRE") == 0)
                {
                    strcpy(GP->FP_PRE, S2);
                }
                else if (strcmp(S1, "FP_GEO") == 0)
                {
                    strcpy(GP->FP_GEO, S2);
                }
                else if (strcmp(S1, "FP_WEATHER") == 0)
                {
                    strcpy(GP->FP_WEATHER, S2);
                }
                else if (strcmp(S1, "START_YEAR") == 0)
                {
                    GP->START_YEAR = atoi(S2);
                }
                else if (strcmp(S1, "START_MONTH") == 0)
                {
                    GP->START_MONTH = atoi(S2);
                }
                else if (strcmp(S1, "START_DAY") == 0)
                {
                    GP->START_DAY = atoi(S2);
                }
                else if (strcmp(S1, "START_HOUR") == 0)
                {
                    GP->START_HOUR = atoi(S2);
                } 
                else if (strcmp(S1, "STEP_TIME") == 0)
                {
                    GP->STEP_TIME = atoi(S2);
                }
                else
                {
                    printf("Unrecognized field!");
                    exit(0);
                }
            }
        }
    }
    fclose(fp);
}

