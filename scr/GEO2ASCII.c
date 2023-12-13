/*
 * SUMMARY:      GEO2ASCII.c
 * USAGE:        transfer the NetCDF geodata into ASCII text format
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  
 * DESCRIP-END.
 * FUNCTIONS:    Export_raster();Export_header();Export_GEO_data()
 * 
 * COMMENTS:
 *  1. this module is aimed to read the data from NetCDF format file and 
 *     transfer and store in the ASCII text files
 *  2. The variable names in the NetCDF file could be DEM
 *     FDR, FAC, STR, OUTLET
 *  3. This module is actually optional, 
 *     aimed to help check the GEO2NC.c module.
 *
 */

/*************************************************************
 * VARIABLEs:
 * int *rdata              - point to the data to be stored into ASCII file
 * FILE *fp_geo            - point to the (ASCII) file
 * int ncols               - number of cell columns of the raster/gridded (2D) data
 * int nrows               - number of cell rows of the raster/gridded (2D) data
 * ST_Header HD            - structure for the ASCII header
 * char FP[]               - ASCII file name 
 * int ncID                - NetCDF file ID
 * char varNAME[]          - variable name in the NetCDF file
 * int varID               - variable ID
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>

#include "GEO_ST.h"
#include "GEO2ASCII.h"

void Export_raster(
    int *rdata,
    FILE *fp_geo,
    int ncols,
    int nrows
)
{
    /************************
     * write the raster/gridded data in to ASCII text file
     * rdata locates the first value of a sequence data
    */
    int i, j;
    for (i = 0; i < nrows; i++)
    {
        for (j = 0; j < ncols; j++)
        {
            fprintf(
                fp_geo, "%d ", *(rdata + i * ncols + j));
        }
        fprintf(fp_geo, "\n");
    }
    
}

void Export_header(
    FILE *fp_geo,
    ST_Header HD
)
{
    /*******************************
     * write the header info into text file
    */
    fprintf(fp_geo, "%-14s%d\n", "ncols", HD.ncols);
    fprintf(fp_geo, "%-14s%d\n", "nrows", HD.nrows);
    fprintf(fp_geo, "%-14s%-.12f\n", "xllcorner", HD.xllcorner);
    fprintf(fp_geo, "%-14s%-.12f\n", "yllcorner", HD.yllcorner);
    fprintf(fp_geo, "%-14s%-.12f\n", "cellsize", HD.cellsize);
    fprintf(fp_geo, "%-14s%-d\n", "NODATA_value", HD.NODATA_value);
}

void Export_GEO_data(
    int ncID,
    char varNAME[],
    ST_Header HD,
    char FP[]
)
{
    /**************************************
     * write variable from a NetCDF file into 
     * independent ASCII text file, including
     * both header and data part
    */
    int varID;
    int *rdata;
    nc_inq_varid(ncID, varNAME, &varID);
    nc_get_att_int(ncID, varID, "NODATA_value", &HD.NODATA_value);
    printf("NODATA_value: %d\n", HD.NODATA_value);
    rdata = (int *)malloc(sizeof(int) * HD.ncols * HD.nrows);
    nc_get_var_int(ncID, varID, rdata);

    FILE *fp_out;
    if ((fp_out = fopen(FP, "w")) == NULL)
    {
        printf("Program terminated: cannot create or open output file\n");
        exit(0);
    }
    Export_header(fp_out, HD);
    Export_raster(rdata, fp_out, HD.ncols, HD.nrows);
    fclose(fp_out);
    free(rdata);
}
