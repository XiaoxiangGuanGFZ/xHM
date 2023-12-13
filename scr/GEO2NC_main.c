#include <stdio.h>
#include <stdlib.h>
#include <netcdf.h>
#include <string.h>
#include <math.h>
#include "GEO_ST.h"
#include "GEO2NC.h"
#include "GEO_para.h"

void handle_error(int status);

int main(int argc, char * argv[])
{
    char IO[2] = "-I";
    strcpy(IO, *(++argv));
    if (strcmp(IO, "-I") == 0) 
    {
        printf("---- Transfer 2D array data from ASCII to NetCDF format! ----\n");
    } 
    else
    {
        printf("---- Transfer 2D array data from NetCDF to ASCII format! ----\n");
    }

    ST_GEO2NC_Para GP;
    Import_para(*(++argv), &GP, 1);
    Para_check(&GP);
    if (GP.FP_GEONC[0] == '\0')
    {
        printf("Field FP_GEONC parameter file is empty! Program failed\n");
        exit(0);
    }
    
    if (strcmp(IO, "-I") == 0)
    {
        // DEM
        ST_Header HD_dem;
        int *rdata_dem;
        if (GP.FP_DEM[0] != '\0')
        {
            Import_data(GP.FP_DEM, &rdata_dem, &HD_dem, 1);
            /**
             * transfer the pointer to a pointer (address of an address)
             * in order to munipulate the rdata_dem
             *  (bring the results/cahnges) from function execution
             */
        }
        
        // flow direction: fdr
        ST_Header HD_fdr;
        int *rdata_fdr;
        if (GP.FP_FDR[0] != '\0')
        {
            Import_data(GP.FP_FDR, &rdata_fdr, &HD_fdr, 0);
        }

        // flow accumulation: fac
        ST_Header HD_fac;
        int *rdata_fac;
        if (GP.FP_FAC[0] != '\0')
        {
            Import_data(GP.FP_FAC, &rdata_fac, &HD_fac, 0);
        }

        // stream: str
        ST_Header HD_str;
        int *rdata_str;
        if (GP.FP_STR[0] != '\0')
        {
            Import_data(GP.FP_STR, &rdata_str, &HD_str, 0);
        }
        
        // outlet
        ST_Header HD_outlet;
        int *rdata_outlet;
        if (GP.FP_OUTLET != '\0')
        {
            Import_data(GP.FP_OUTLET, &rdata_outlet, &HD_outlet, 0);
        }
        
        /************* create nc file ****************/
        int i, j;
        double *data_lon;
        double *data_lat;
        data_lon = (double *)malloc(sizeof(double) * HD_dem.ncols);
        data_lat = (double *)malloc(sizeof(double) * HD_dem.nrows);

        for (j = 0; j < HD_dem.ncols; j++)
        {
            *(data_lon + j) = HD_dem.xllcorner + HD_dem.cellsize * j;
        }
        for (i = 0; i < HD_dem.nrows; i++)
        {
            *(data_lat + i) = HD_dem.yllcorner + HD_dem.cellsize * (HD_dem.nrows - i - 1);
        }
        

        int IO_status;
        int ncID;
        int dimID_lon, dimID_lat;
        int varID_lon, varID_lat;
        int varID_dem, varID_fdr, varID_fac, varID_str, varID_outlet;
        int old_fill_mode;

        IO_status = nc_create(GP.FP_GEONC, NC_CLOBBER, &ncID);
        if (IO_status != NC_NOERR)
            handle_error(IO_status);
        /**** define mode ****/ 
        nc_set_fill(ncID, NC_FILL, &old_fill_mode);
        nc_def_dim(ncID, "lon", HD_dem.ncols, &dimID_lon);
        nc_def_dim(ncID, "lat", HD_dem.nrows, &dimID_lat);

        int dims[2];
        int fdr_ranges[8] = {1,2,4,8,16,32,64,128};
        dims[0] = dimID_lat;
        dims[1] = dimID_lon;
        nc_def_var(ncID, "lon", NC_DOUBLE, 1, &dimID_lon, &varID_lon);
        nc_def_var(ncID, "lat", NC_DOUBLE, 1, &dimID_lat, &varID_lat);
        nc_def_var(ncID, "DEM", NC_INT, 2, dims, &varID_dem);
        nc_def_var(ncID, "FDR", NC_INT, 2, dims, &varID_fdr);
        nc_def_var(ncID, "FAC", NC_INT, 2, dims, &varID_fac);
        nc_def_var(ncID, "STR", NC_INT, 2, dims, &varID_str);
        nc_def_var(ncID, "OUTLET", NC_INT, 2, dims, &varID_outlet);

        /******** put attributes *******/
        // global attributes
        nc_put_att_int(ncID, NC_GLOBAL, "ncols", NC_INT, 1, &HD_dem.ncols);
        nc_put_att_int(ncID, NC_GLOBAL, "nrows", NC_INT, 1, &HD_dem.nrows);
        nc_put_att_double(ncID, NC_GLOBAL, "xllcorner", NC_DOUBLE, 1, &HD_dem.xllcorner);
        nc_put_att_double(ncID, NC_GLOBAL, "yllcorner", NC_DOUBLE, 1, &HD_dem.yllcorner);
        nc_put_att_double(ncID, NC_GLOBAL, "cellsize", NC_DOUBLE, 1, &HD_dem.cellsize);
        
        nc_put_att_text(ncID, NC_GLOBAL, "Projection", 30, GP.Projection);
        nc_put_att_text(ncID, NC_GLOBAL, "Spheroid", 20L, GP.Spheroid);
        nc_put_att_text(ncID, NC_GLOBAL, "Units", 20L, GP.Units);
        nc_put_att_text(ncID, NC_GLOBAL, "Zunits", 20L, GP.Zunits);
        nc_put_att_text(ncID, NC_GLOBAL, "Datum", 20L, GP.Datum);
        nc_put_att_int(ncID, NC_GLOBAL, "cellsize_m", NC_INT,1, &GP.cellsize_m);
        
        // DEM 
        nc_put_att_text(ncID, varID_dem, "long_name", 40L, "Digital Elevation Model");
        nc_put_att_text(ncID, varID_dem, "units", 40L, "meters");
        nc_put_att_int(ncID, varID_dem, "missing_value", NC_INT, 1, &HD_dem.NODATA_value);

        // flow direction: fdr
        nc_put_att_text(ncID, varID_fdr, "long_name", 40L, "flow direction");
        nc_put_att_text(ncID, varID_fdr, "units", 40L, "NULL");
        nc_put_att_int(ncID, varID_fdr, "missing_value", NC_INT, 1, &HD_dem.NODATA_value);
        nc_put_att_int(ncID, varID_fdr, "valid_range", NC_INT, 8, fdr_ranges);
        nc_put_att_text(ncID, varID_fdr, "description", 200L, \
            "D8 direction type: 1(East/Right); 2(SouthEast/LowerRight); 4(South/Down); 8(SouthWest/Lowerleft); 16(West/Left); 32(NorthWest/UpperLeft); 64(North/Up); 128(NorthEst/UpperRight)");
        
        // flow accumulation: fac
        nc_put_att_text(ncID, varID_fac, "long_name", 40L, "flow accumulation");
        nc_put_att_text(ncID, varID_fac, "units", 40L, "number of cells");
        nc_put_att_int(ncID, varID_fac, "missing_value", NC_INT, 1, &HD_dem.NODATA_value);

        // stream: str
        nc_put_att_text(ncID, varID_str, "long_name", 40L, "stream");
        nc_put_att_int(ncID, varID_str, "missing_value", NC_INT, 1, &HD_dem.NODATA_value);
        nc_put_att_int(ncID, varID_str, "valid_range", NC_INT, 1, (int[]){1});
        nc_put_att_text(ncID, varID_str, "description", 40L, "1: this cell is the stream");

        // outlet
        nc_put_att_text(ncID, varID_outlet, "long_name", 40L, "outlet of watersheds");
        nc_put_att_int(ncID, varID_outlet, "missing_value", NC_INT, 1, &HD_dem.NODATA_value);
        nc_put_att_int(ncID, varID_outlet, "valid_range", NC_INT, 1, (int[]){1});
        nc_put_att_text(ncID, varID_outlet, "description", 40L, "1: this cell is an outlet");


        nc_enddef(ncID);
        /*********  data mode *******/
        size_t start[2] = {0, 0};
        size_t counts[2];
        counts[0] = HD_dem.nrows;
        counts[1] = HD_dem.ncols;
        nc_put_var_double(ncID, varID_lon, data_lon);
        nc_put_var_double(ncID, varID_lat, data_lat);

        if (GP.FP_DEM[0] != '\0')
        {
            nc_put_vara_int(ncID, varID_dem, start, counts, rdata_dem);
        }
        if (GP.FP_FDR[0] != '\0')
        {
            nc_put_vara_int(ncID, varID_fdr, start, counts, rdata_fdr);
        }
        if (GP.FP_FAC[0] != '\0')
        {
            nc_put_vara_int(ncID, varID_fac, start, counts, rdata_fac);
        }
        
        if (GP.FP_STR[0] != '\0')
        {
            nc_put_vara_int(ncID, varID_str, start, counts, rdata_str);
        }
        
        if (GP.FP_OUTLET[0] != '\0')
        {
            nc_put_vara_int(ncID, varID_outlet, start, counts, rdata_outlet);
        }
        
        nc_close(ncID);
    }
    
    return 1;
}

void handle_error(int status)
{
    /*****************************
     * error handling function for 
     * netcdf data reading and writing
    */
    if (status != NC_NOERR)
    {
        fprintf(stderr, "%s\n", nc_strerror(status));
        exit(-1);
    }
}
