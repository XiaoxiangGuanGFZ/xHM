/*
 * SUMMARY:      GEO2NC.c
 * USAGE:        import the ASCII text geodata into NetCDF format
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  read geodata from ASCII files and store into NetCDF format
 * DESCRIP-END.
 * FUNCTIONS:    Import_data(); Import_raster(); Header_read(); 
 * 
 * COMMENTS:
 * 
 *
 */

/*******************************************************************************
 * VARIABLEs:
 * char FP[]                 - file path and file name of the ASCII text file
 * FILE *fp_geo              - file pointer (to the ASCII text file)
 * ST_GEO2NC_Para *gp        - point to the parameter structure
 * int display               - 1: print info (during the program) on the screen 
 * ST_Header *HD             - point to the header structure
 * int *rdata                - point to the first value of the 2D raster data
 * 
 * 
********************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "GEO_ST.h"
#include "GEO2NC.h"

int Header_read(
    FILE *fp_geo,
    ST_Header *HD,
    int display)
{
    /*******************************************
     * import the header lines in an ASCII
     * file containg geodata, which usually has 6 rows:
     * - ncols
     * - nrows
     * - xllcorner
     * - yllcorner
     * - cellsize
     * - NODATA_value
    */
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
    /********************************************
     * import the 2D array (gridded data)
     * the data part from an ASCII file, excluding 
     * the header rows
    */
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
    /*********************************************
     * import the full data (both headers and 2D aray) 
     * from an ASCII geodata file
    */
    FILE *fp;
    int IO_status;
    // first, open the file given its path and filename
    if ((fp = fopen(FP, "r")) == NULL)
    {
        printf("failed to open %s!\n", FP);
        exit(0);
    }

    // import the header rows 
    IO_status = Header_read(fp, fp_header, display);
    if (IO_status == 1)
    {
        // if the header rows are imported successfully,
        // import the data part (2D raster array)
        *rdata = (int *)malloc(sizeof(int) * fp_header->ncols * fp_header->nrows);
        Import_raster(*rdata, fp, fp_header->ncols, fp_header->nrows);
    }
    fclose(fp); // close the ASCII file
    return 1;
}




