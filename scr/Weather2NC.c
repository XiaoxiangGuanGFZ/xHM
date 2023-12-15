
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Weather2NC_ST.h"
#include "Weather2NC.h"

void Import_weather_para(
    char FP[],
    ST_weather_para *GP,
    int display)
{
    FILE *fp;
    if ((fp = fopen(FP, "r")) == NULL)
    {
        printf("cannot open %s file\n", FP);
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
                if (display == 1)
                {
                    printf("%s,%s\n", S1, S2);
                }
                if (strcmp(S1, "FP_PRE") == 0)
                {
                    strcpy(GP->FP_PRE, S2);
                }
                else if (strcmp(S1, "FP_COOR") == 0)
                {
                    strcpy(GP->FP_COOR, S2);
                }
                else if (strcmp(S1, "FP_WEATHER") == 0)
                {
                    strcpy(GP->FP_WEATHER, S2);
                }
                else if (strcmp(S1, "FP_GEO") == 0)
                {
                    strcpy(GP->FP_GEO, S2);
                } 
                else if (strcmp(S1, "FP_WEA_OUT") == 0)
                {
                    strcpy(GP->FP_WEA_OUT, S2);
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
                else if (strcmp(S1, "STEP") == 0)
                {
                    GP->STEP = atoi(S2);
                } 
                else if (strcmp(S1, "SITES") == 0)
                {
                    GP->SITES = atoi(S2);
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

void Import_COOR(
    char FP_COOR[],
    ST_COOR *coor)
{
    FILE *fp;
    if ((fp = fopen(FP_COOR, "r")) == NULL)
    {
        printf("cannot open %s file\n", FP_COOR);
        exit(0);
    }
    int i, j;
    char row[MAXCHAR];
    j = 0;
    while (fgets(row, MAXCHAR, fp) != NULL)
    {
        for (i = 0; i < strlen(row); i++)
        {
            /* remove all the characters after # */
            if (row[i] == '#')
            {
                row[i] = '\0'; // replace the '#' with '\0', end sign.
            }
        }
        if (row != NULL && strlen(row) > 1 && row[0] != '#')
        {
            if (sscanf(
                    row, "%d,%lf,%lf,%d",
                    &((coor + j)->ID), &((coor + j)->lon), &((coor + j)->lat), &((coor + j)->DEM)) != 4)
            {
                printf("Reading coor file %s, importing %dth row failed!\n", FP_COOR, j + 1);
            }
            j = j + 1;
        }
    }
    fclose(fp);
}

void Import_weather_data(
    char FP[],
    int *rdata,
    int SITES,
    int *nrows
)
{
    FILE *fp;
    if ((fp = fopen(FP, "r")) == NULL)
    {
        printf("Failed in opening file %s", FP);
        exit(0);
    }
    int i, j;
    j = 0;
    char *token;
    char row[MAXCHAR];
    while (fgets(row, MAXCHAR, fp) != NULL)
    {
        *(rdata + j * SITES) = atoi(strtok(row, ","));
        for (i = 1; i < SITES; i++)
        {
            *(rdata + j * SITES + i) = atoi(strtok(NULL, ","));
        }
        j = j + 1;
    }
    *nrows = j;
    fclose(fp);
}
