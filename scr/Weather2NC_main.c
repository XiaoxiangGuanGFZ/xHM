#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include <math.h>
#include "Weather2NC_ST.h"
#include "Weather2NC.h"

#include "GEO_ST.h"

#define MAXrows 10000
#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return e;}

double inv_distance(
    double lon1,
    double lon2,
    double lat1,
    double lat2
);
int copy_global_attributes(int input_ncid, int output_ncid);

void main(int argc, char * argv[]){
    ST_weather_para GP;
    Import_weather_para(*(++argv), &GP, 1);
    
    ST_COOR *coor;
    coor = (ST_COOR *)malloc(
        sizeof(ST_COOR) * GP.SITES
    );

    Import_COOR(GP.FP_COOR, coor);
    // int i;
    // for (i = 0; i < GP.SITES; i++)
    // {
    //     printf("%2d,%7.3f,%7.3f,%4d\n", (coor+i)->ID, (coor+i)->lon, (coor+i)->lat, (coor+i)->DEM);
    // }
    int nrows_wea;
    int *rdata;
    rdata = (int *)malloc(MAXrows * GP.SITES * sizeof(int));
    Import_weather_data(
        GP.FP_PRE,
        rdata,
        GP.SITES,
        &nrows_wea);
    printf("Rows: %d\n", nrows_wea);

    // int j;
    // for (i = 193; i < 220; i++)
    // {
    //     for (j = 0; j < GP.SITES; j++)
    //     {
    //         printf("%3d ", *(rdata + GP.SITES * i + j));
    //     }
    //     printf("\n");
    // }
    int ncID;
    ST_Header GEO_header;
    nc_open(GP.FP_GEO, NC_NOWRITE, &ncID);
    nc_get_att_int(ncID, NC_GLOBAL, "ncols", &GEO_header.ncols);
    nc_get_att_int(ncID, NC_GLOBAL, "nrows", &GEO_header.nrows);
    nc_get_att_double(ncID, NC_GLOBAL, "xllcorner", &GEO_header.xllcorner);
    nc_get_att_double(ncID, NC_GLOBAL, "yllcorner", &GEO_header.yllcorner);
    nc_get_att_double(ncID, NC_GLOBAL, "cellsize", &GEO_header.cellsize);
    printf("ncols: %d\nnrows: %d\nxllcorner: %.12f\nyllcorner: %.12f\ncellsize: %.12f\n",
            GEO_header.ncols, GEO_header.nrows, GEO_header.xllcorner, GEO_header.yllcorner, GEO_header.cellsize
        );
    int varID_lon, varID_lat, varID_DEM;
    double *data_lon;
    double *data_lat;
    int *data_DEM;
    data_lon = (double *)malloc(sizeof(double) * GEO_header.ncols);
    data_lat = (double *)malloc(sizeof(double) * GEO_header.nrows);
    data_DEM = (int *)malloc(sizeof(int) * GEO_header.nrows * GEO_header.ncols);
    nc_inq_varid(ncID, "lon", &varID_lon);
    nc_get_var_double(ncID, varID_lon, data_lon);

    nc_inq_varid(ncID, "lat", &varID_lat);
    nc_get_var_double(ncID, varID_lat, data_lat);

    nc_inq_varid(ncID, "DEM", &varID_DEM);
    nc_get_var_int(ncID, varID_DEM, data_DEM);
    nc_get_att_int(ncID, varID_DEM, "NODATA_value", &GEO_header.NODATA_value);

    // for (int i = 0; i < GEO_header.nrows; i++)
    // {
    //     printf("%0.3f ", *(data_lat + i));
    // }
    
    double *weight;
    double dis_sum;
    double *dis;
    dis = (double *)malloc(sizeof(double) * GP.SITES);
    weight = (double *)malloc(sizeof(double) * GEO_header.ncols * GEO_header.nrows * GP.SITES);
    for (int i = 0; i < GEO_header.nrows; i++)
    {
        for (int j = 0; j < GEO_header.ncols; j++)
        {
            dis_sum = 0.0;
            for (int s = 0; s < GP.SITES; s++)
            {
                *(dis + s) = inv_distance(
                    *(data_lon + j), (coor + s)->lon,
                    *(data_lat + i), (coor + s)->lat
                );
                dis_sum = dis_sum + *(dis + s);
            }
            for (int s = 0; s < GP.SITES; s++)
            {
                *(weight + i * GEO_header.ncols * GP.SITES + j * GP.SITES + s) = *(dis + s) / dis_sum;
            }
        }
    }
    
    /**************************/
    int ncID_weather;
    int dimID_lon, dimID_lat, dimID_time;
    int varID_time, varID_PRE; // varID_lon, varID_lat,
    nc_create(GP.FP_WEA_OUT, NC_CLOBBER, &ncID_weather);
    
    nc_def_dim(ncID_weather, "lon", GEO_header.ncols, &dimID_lon);
    nc_def_dim(ncID_weather, "lat", GEO_header.nrows, &dimID_lat);
    nc_def_dim(ncID_weather, "time", NC_UNLIMITED, &dimID_time);
    int dims[3];
    dims[0] = dimID_time;
    dims[1] = dimID_lat;
    dims[2] = dimID_lon;
    
    nc_def_var(ncID_weather, "lon", NC_DOUBLE, 1, &dimID_lon, &varID_lon);
    nc_def_var(ncID_weather, "lat", NC_DOUBLE, 1, &dimID_lat, &varID_lat);
    // nc_def_var(ncID_weather, "time", NC_INT, 1, &dimID_time, &varID_time);
    nc_def_var(ncID_weather, "PRE", NC_INT, 3, dims, &varID_PRE);
    copy_global_attributes(ncID, ncID_weather);

    nc_put_att_int(ncID_weather, varID_PRE, "NODATA_value", NC_INT, 1, &GEO_header.NODATA_value);
    nc_enddef(ncID_weather);
    /******* data mode ****
     * interpolate weather data from sites to the spatial domain
     * based on the method of Inverse Distance Weighting
     * 
    */
    size_t index[3] = {0,0,0};
    int rr;
    double rr_f;
    // nrows_wea = 30;
    for (int i = 0; i < GEO_header.nrows; i++)
    {
        index[1] = i;
        for (int j = 0; j < GEO_header.ncols; j++)
        {
            index[2] = j;
            if (*(data_DEM + i * GEO_header.ncols + j) == GEO_header.NODATA_value)
            {
                for (size_t t = 0; t < nrows_wea; t++)
                {
                    index[0] = t;
                    nc_put_var1_int(
                        ncID_weather, varID_PRE, index, &(GEO_header.NODATA_value)
                    );
                }
            } else {
                for (size_t t = 0; t < nrows_wea; t++)
                {
                    index[0] = t;
                    rr = 0;
                    rr_f = 0.0;
                    for (size_t s = 0; s < GP.SITES; s++)
                    {
                        rr_f += \
                        *(rdata + t * GP.SITES + s) * *(weight + i * GEO_header.ncols * GP.SITES + j * GP.SITES + s);
                    }
                    rr = (int) rr_f;
                    nc_put_var1_int(
                        ncID_weather, varID_PRE, index, &rr
                    );
                }
            }
            printf("row: %3d ncol: %3d\n", i, j);
        }
    }

    nc_close(ncID);
    nc_close(ncID_weather);

}

double inv_distance(
    double lon1,
    double lon2,
    double lat1,
    double lat2
)
{
    double idis;
    idis = 1.0 / sqrt(pow(lon1 - lon2, 2) + pow(lat1 - lat2, 2));
    return idis;
}

int copy_global_attributes(int input_ncid, int output_ncid) {
    int status, num_atts, i;
    char attname[NC_MAX_NAME + 1];
    nc_type xtype;
    size_t attlen;
    void *attvalue;

    // Get the number of attributes for NC_GLOBAL
    if ((status = nc_inq_natts(input_ncid, &num_atts)) != NC_NOERR) ERR(status);

    // Loop through each attribute
    for (i = 0; i < num_atts; i++) {
        // Get attribute information
        if ((status = nc_inq_attname(input_ncid, NC_GLOBAL, i, attname)) != NC_NOERR) ERR(status);
        if ((status = nc_inq_att(input_ncid, NC_GLOBAL, attname, &xtype, &attlen)) != NC_NOERR) ERR(status);

        // Allocate memory for attribute value
        attvalue = malloc(attlen);
        if (attvalue == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            return NC_ENOMEM;
        }

        // Read attribute value
        if ((status = nc_get_att(input_ncid, NC_GLOBAL, attname, attvalue)) != NC_NOERR) {
            free(attvalue);
            ERR(status);
        }

        // Write attribute to the output file
        if ((status = nc_put_att(output_ncid, NC_GLOBAL, attname, xtype, attlen, attvalue)) != NC_NOERR) {
            free(attvalue);
            ERR(status);
        }

        // Free allocated memory
        free(attvalue);
    }

    return NC_NOERR;
}
