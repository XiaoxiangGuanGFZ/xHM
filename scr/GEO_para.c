/*
 * SUMMARY:      GEO_para.c
 * USAGE:        read the parameter file for GEO program
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  
 * DESCRIP-END.
 * FUNCTIONS:    Import_para(); Para_check()
 * 
 * COMMENTS:
 * 
 *
 */

/*************************************************************
 * VARIABLEs:
 * char FP[]                - file path and name the parameter file
 * ST_GEO2NC_Para *gp       - point to a parameter structure
 * 
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Constants.h"
#include "GEO_ST.h"
#include "GEO_para.h"

void Import_para(
    char FP[],
    ST_GEO2NC_Para *gp,
    int display)
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
                if (display == 1)
                {
                    printf("%s,%s\n", S1, S2);
                }
                if (strcmp(S1, "FP_DEM") == 0)
                {
                    strcpy(gp->FP_DEM, S2);
                }
                else if (strcmp(S1, "FP_FDR") == 0)
                {
                    strcpy(gp->FP_FDR, S2);
                }
                else if (strcmp(S1, "FP_FAC") == 0)
                {
                    strcpy(gp->FP_FAC, S2);
                }
                else if (strcmp(S1, "FP_STR") == 0)
                {
                    strcpy(gp->FP_STR, S2);
                }
                else if (strcmp(S1, "FP_OUTLET") == 0)
                {
                    strcpy(gp->FP_OUTLET, S2);
                }
                else if (strcmp(S1, "FP_VEGTYPE") == 0)
                {
                    strcpy(gp->FP_VEGTYPE, S2);
                }
                else if (strcmp(S1, "FP_VEGFRAC") == 0)
                {
                    strcpy(gp->FP_VEGFRAC, S2);
                }
                else if (strcmp(S1, "FP_SOILTYPE") == 0)
                {
                    strcpy(gp->FP_SOILTYPE, S2);
                }
                else if (strcmp(S1, "FP_GEONC") == 0)
                {
                    strcpy(gp->FP_GEONC, S2);
                }
                else if (strcmp(S1, "Projection") == 0)
                {
                    strcpy(gp->Projection, S2);
                }
                else if (strcmp(S1, "Datum") == 0)
                {
                    strcpy(gp->Datum, S2);
                }
                else if (strcmp(S1, "Spheroid") == 0)
                {
                    strcpy(gp->Spheroid, S2);
                }
                else if (strcmp(S1, "Units") == 0)
                {
                    strcpy(gp->Units, S2);
                }
                else if (strcmp(S1, "Zunits") == 0)
                {
                    strcpy(gp->Zunits, S2);
                }
                else if (strcmp(S1, "cellsize_m") == 0)
                {
                    gp->cellsize_m = atoi(S2);
                }
                else
                {
                    printf("Unrecognized field!");
                    exit(0);
                }
            }
        }
    }
}

void Para_check(
    ST_GEO2NC_Para *gp
)
{
    /*********************************************
     * check the mandatory variables in the para(meter) file
     * if any one of the required field is empty, the 
     * program is terminated.  
    */
    if (gp->FP_GEONC[0] == '\0'){
        printf("FP_GEONC field is empty! Program failed!\n");
        exit(0);
    }
    if (gp->Spheroid[0] == '\0'){
        printf("Spheroid field is empty! Program failed!\n");
        exit(0);
    }
    if (gp->Units[0] == '\0'){
        printf("Units field is empty! Program failed!\n");
        exit(0);
    }
    if (gp->Zunits[0] == '\0'){
        printf("Zunits field is empty! Program failed!\n");
        exit(0);
    }
    if (gp->Projection[0] == '\0'){
        printf("Projection field is empty! Program failed!\n");
        exit(0);
    }
    if (gp->Datum[0] == '\0'){
        printf("Datum field is empty! Program failed!\n");
        exit(0);
    }
    if (gp->cellsize_m < 0){
        printf("cellsize less than 0. Program failed!\n");
        exit(0);
    }
}

