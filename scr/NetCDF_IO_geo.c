
/*
 * SUMMARY:      NetCDF_IO_geo.c
 * USAGE:        save intermediate variables from model simulation to a NC file
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  export the variables from model simulation
 * DESCRIP-END.
 * FUNCTIONS:    Write2NC()
 * 
 * COMMENTS:
 * 
 *
 * REFERENCES:
 *
 */

/*****************************************************************
 * VARIABLEs:
 * char varNAME[]                   - the name of the variable
 * char att_unit[]                  - the unit of the variable
 * char att_longname[]              - the long name of the variable
 * char FP_GEO[]                    - the file path of the GEO file
 * char FP_output[]                 - the file path of the output (where to save the variable) 
 * double **rdata                   - variable data 
 * time_t start_time                - the starting time of model running
 * int step_time                    - the step of model simulation: hours
 * int ts_length                    - the length of the time variable (time series)
 * 
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include <time.h>
#include "Constants.h"
#include "GEO_ST.h"
#include "NC_copy_global_att.h"

void Write2NC(
    char varNAME[],
    char att_unit[],
    char att_longname[],
    double scale_factor,
    char FP_GEO[],
    char FP_output[],
    int **rdata,
    int step_time,
    int ts_length,
    int START_YEAR,
    int START_MONTH,
    int START_DAY,
    int START_HOUR
)
{
    int status_nc;
    int ncID_GEO;
    status_nc = nc_open(FP_GEO, NC_NOWRITE, &ncID_GEO);
    if (status_nc != NC_NOERR)
    {
        printf("error in opening file%s: %s\n", FP_GEO, nc_strerror(status_nc));
        exit(-1);
    }
    ST_Header HD;
    // int ncols,nrows;
    // double xllcorner, yllcorner, cellsize;
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "ncols", &HD.ncols);
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "nrows", &HD.nrows);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "xllcorner", &HD.xllcorner);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "yllcorner", &HD.yllcorner);
    nc_get_att_double(ncID_GEO, NC_GLOBAL, "cellsize", &HD.cellsize);
    int varID_lon, varID_lat, varID_DEM;
    double *data_lon;
    double *data_lat;
    int *data_DEM;
    data_lon = (double *)malloc(sizeof(double) * HD.ncols);
    data_lat = (double *)malloc(sizeof(double) * HD.nrows);
    data_DEM = (int *)malloc(sizeof(int) * HD.nrows * HD.ncols);

    nc_inq_varid(ncID_GEO, "DEM", &varID_DEM);
    nc_get_att_int(ncID_GEO, varID_DEM, "NODATA_value", &HD.NODATA_value);
    nc_get_var_int(ncID_GEO, varID_DEM, data_DEM);

    nc_inq_varid(ncID_GEO, "lon", &varID_lon);
    nc_get_var_double(ncID_GEO, varID_lon, data_lon);

    nc_inq_varid(ncID_GEO, "lat", &varID_lat);
    nc_get_var_double(ncID_GEO, varID_lat, data_lat);
    
    /******** output variable to be stored in NetCDF file *******/
    // define the NC file: variables, dimensions, attributes
    int ncID_out;
    status_nc = nc_create(FP_output, NC_CLOBBER, &ncID_out);
    if (status_nc != NC_NOERR)
    {
        printf("error in opening file%s: %s\n", FP_output, nc_strerror(status_nc));
        exit(-1);
    }
    int varID_out, varID_ts;
    int dimID_lon, dimID_lat, dimID_time;
    nc_def_dim(ncID_out, "time", NC_UNLIMITED, &dimID_time);
    nc_def_dim(ncID_out, "lon", HD.ncols, &dimID_lon);
    nc_def_dim(ncID_out, "lat", HD.nrows, &dimID_lat);

    int dims[3];
    dims[0] = dimID_time;
    dims[1] = dimID_lat;
    dims[2] = dimID_lon;
    nc_def_var(ncID_out, "lon", NC_DOUBLE, 1, &dimID_lon, &varID_lon);
    nc_def_var(ncID_out, "lat", NC_DOUBLE, 1, &dimID_lat, &varID_lat);
    nc_def_var(ncID_out, "time", NC_INT, 1, &dimID_time, &varID_ts); // long int (64-byte)
    
    nc_def_var(ncID_out, varNAME, NC_INT, 3, dims, &varID_out);
    nc_put_att_text(ncID_out, varID_out, "Units", 40L, att_unit);
    nc_put_att_text(ncID_out, varID_out, "long_name", 100L, att_longname);
    nc_put_att_double(ncID_out, varID_out, "scale_factor", NC_DOUBLE, 1, &scale_factor);
    nc_put_att_int(ncID_out, varID_out, "NODATA_value", NC_INT, 1, &HD.NODATA_value);
    nc_put_att_int(ncID_out, varID_out, "STEP_TIME(hours)", NC_INT, 1, &step_time);
    nc_put_att_int(ncID_out, varID_out, "counts", NC_INT, 1, &ts_length);

    copy_global_attributes(ncID_GEO, ncID_out);
    nc_enddef(ncID_out);
    
    // write data
    // the two dimensions: lon and lat
    nc_put_var_double(ncID_out, varID_lon, data_lon);
    nc_put_var_double(ncID_out, varID_lat, data_lat);
    
    // the ourput variable: 3D
    if (*(data_DEM + 0) == HD.NODATA_value && *(*rdata + 0) != HD.NODATA_value)
    {
        /***
         * check the NODATA_value of the outputted variable
        */
        for (size_t i = 0; i < HD.nrows; i++)
        {
            for (size_t j = 0; j < HD.ncols; j++)
            {
                if (*(data_DEM + i * HD.ncols + j) == HD.NODATA_value)
                {
                    for (size_t t = 0; t < ts_length; t++)
                    {
                        *(*rdata + t * HD.ncols * HD.nrows + i * HD.ncols + j) = HD.NODATA_value;
                    }
                }
            }
        }
    }

    int out_start[3] = {0,0,0};
    int out_count[3];
    out_count[0] = ts_length; 
    out_count[1] = HD.nrows;
    out_count[2] = HD.ncols;
    status_nc = nc_put_vara_int(ncID_out, varID_out, out_start, out_count, *rdata);
    if (status_nc != NC_NOERR)
    {
        printf("Error writing variable data: %s\n", nc_strerror(status_nc));
        exit(-1);
    }


    // the time variable in NetCDF
    time_t start_time;
    struct tm tm_start;
    tm_start.tm_hour = START_HOUR + 1;
    tm_start.tm_min = 0;
    tm_start.tm_sec = 0;
    tm_start.tm_mday = START_DAY;
    tm_start.tm_mon = START_MONTH - 1;
    tm_start.tm_year = START_YEAR - 1900;
    // Daylight Saving Time not in effect;Specify UTC time zone
    tm_start.tm_isdst = 0; 
    start_time = mktime(&tm_start);

    time_t *data_time;
    data_time = (time_t *)malloc(sizeof(time_t) * ts_length);
    for (size_t i = 0; i < ts_length; i++)
    {
        *(data_time + i) = start_time + 3600 * step_time * i;
    }
    int start = 0;
    status_nc = nc_put_vara_long(ncID_out, varID_ts, &start, &ts_length, data_time);
    handle_error(status_nc, FP_output);
    
    nc_close(ncID_GEO);
    nc_close(ncID_out);
}

