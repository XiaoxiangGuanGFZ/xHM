/*
 * SUMMARY:      GEO2NC.c
 * USAGE:        
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  
 * DESCRIP-END.
 * FUNCTIONS:    
 * 
 * COMMENTS:
 * 
 *
 * REFERENCES:
 * 
 *
 */

/*******************************************************************************
 * VARIABLEs:
 * 
 * 
 * 
 * 
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <netcdf.h>
#include <math.h>
#include "GEO2NC.h"
#include <string.h>

void Import_para(
    char FP[],
    ST_Para *gp,
    int display    
)
{
    char row[MAXCHAR];
    char S1[MAXCHAR];
    char S2[MAXCHAR];

    FILE *fp;
    if ((fp=fopen(FP, "r")) == NULL) {
        printf("cannot open global parameter file\n");
        exit(0);
    }

    while (fgets(row, MAXCHAR, fp) != NULL)
    {
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

void Para_check(
    ST_Para *gp
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


void handle_error(int status)
{
    if (status != NC_NOERR)
    {
        fprintf(stderr, "%s\n", nc_strerror(status));
        exit(-1);
    }
}

void print2D(
    int *ptr,
    int m,
    int n
)
{
    int i,j;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            printf(
                "%.2f\t", *((ptr + i * n)+j)
            );
        }
        printf("\n");
    }    
}

int Header_read(
    FILE *fp_geo,
    ST_Header *HD,
    int display)
{
    char row[MAXCHAR];
    char s1[MAXCHAR];

    if (fgets(row, MAXCHAR, fp_geo) != NULL)
    {
        sscanf(row, "%s %d", s1, &HD->ncols);
        if (display == 1)
        {
            printf("%s: %d\n", s1, HD->ncols);
        }
        if (fgets(row, MAXCHAR, fp_geo) != NULL)
        {
            sscanf(row, "%s %d", s1, &HD->nrows);
            if (display == 1)
            {
                printf("%s: %d\n", s1, HD->nrows);
            }
            if (fgets(row, MAXCHAR, fp_geo) != NULL)
            {
                sscanf(row, "%s %lf", s1, &HD->xllcorner);
                if (display == 1)
                {
                    printf("%s: %.12lf\n", s1, HD->xllcorner);
                }
                if (fgets(row, MAXCHAR, fp_geo) != NULL)
                {
                    sscanf(row, "%s %lf", s1, &HD->yllcorner);
                    if (display == 1)
                    {
                        printf("%s: %.12lf\n", s1, HD->yllcorner);
                    }
                    if (fgets(row, MAXCHAR, fp_geo) != NULL)
                    {
                        sscanf(row, "%s %lf", s1, &HD->cellsize);
                        if (display == 1)
                        {
                            printf("%s: %.12lf\n", s1, HD->cellsize);
                        }
                        if (fgets(row, MAXCHAR, fp_geo) != NULL)
                        {
                            sscanf(row, "%s %d", s1, &HD->NODATA_value);
                            if (display == 1)
                            {
                                printf("%s: %d\n", s1, HD->NODATA_value);
                            }
                        }
                        else
                        {
                            printf("reading the 5th row in DEM data file failed!\n");
                            exit(0);
                        }
                    }
                    else
                    {
                        printf("reading the 5th row in DEM data file failed!\n");
                        exit(0);
                    }
                }
                else
                {
                    printf("reading the 4th row in DEM data file failed!\n");
                    exit(0);
                }
            }
            else
            {
                printf("reading the 3rd row in DEM data file failed!\n");
                exit(0);
            }
        }
        else
        {
            printf("reading the 2nd row in DEM data file failed!\n");
            exit(0);
        }
    }
    else
    {
        printf("reading the first row in DEM data file failed!\n");
        exit(0);
    }
    return 1;
}

void Import_raster(
    int *rdata,
    FILE *fp_geo,
    int ncols,
    int nrows
)
{
    
    if (rdata == NULL)
    {
        printf("Error allocating memory");
        exit(-1);
    }
    int i, j;
    j = 0;
    char row[MAXCHAR];
    for (j = 0; j < nrows; j++)
    {
        for (i = 0; i < ncols; i++)
        {
            if (fscanf(fp_geo, "%d", rdata + j * ncols + i) != 1)
            {
                fprintf(stderr, "Error reading integer in %d th row from file.\n", j);
                exit(-2);
            }
        }
    }
}

int Import_data(
    char FP[],
    int **rdata,
    ST_Header *fp_header,
    int display
)
{
    FILE *fp;
    int IO_status;
    if ((fp = fopen(FP, "r")) == NULL)
    {
        printf("failed to open %s!\n", FP);
        exit(0);
    }
    IO_status = Header_read(fp, fp_header, display);
    if (IO_status == 1)
    {
        *rdata = (int *)malloc(sizeof(int) * fp_header->ncols * fp_header->nrows);
        Import_raster(*rdata, fp, fp_header->ncols, fp_header->nrows);
    }
    fclose(fp);
    return 1;
}


