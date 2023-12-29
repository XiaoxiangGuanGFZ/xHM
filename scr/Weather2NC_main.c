#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include <math.h>
#include <time.h>

#include "Weather2NC_ST.h"
#include "Weather2NC.h"
#include "NC_copy_global_att.h"
#include "GEO_ST.h"
#include "Constants.h"
#include "NetCDF_IO_geo.h"

double inv_distance(
    double lon1,
    double lon2,
    double lat1,
    double lat2
);


double tem_correct(
    int air_tem_donor,
    double altitude_donor,
    double altitude_target
);

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
        GP.FP_DATA,
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
    // printf("ncols: %d\nnrows: %d\nxllcorner: %.12f\nyllcorner: %.12f\ncellsize: %.12f\n",
    //         GEO_header.ncols, GEO_header.nrows, GEO_header.xllcorner, GEO_header.yllcorner, GEO_header.cellsize
    //     );
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
    int *rdata_out;
    rdata_out = (int *)malloc(sizeof(int) * GEO_header.ncols * GEO_header.nrows * nrows_wea);
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
    
    int out_int;
    double out_f;
    int cell_dem;
    for (int i = 0; i < GEO_header.nrows; i++)
    {
        for (int j = 0; j < GEO_header.ncols; j++)
        {
            cell_dem = *(data_DEM + i * GEO_header.ncols + j);
            if (cell_dem == GEO_header.NODATA_value)
            {
                for (size_t t = 0; t < nrows_wea; t++)
                {
                    *(rdata_out + t * GEO_header.ncols * GEO_header.nrows + i * GEO_header.ncols + j) = GEO_header.NODATA_value;
                }
            } else {
                for (size_t t = 0; t < nrows_wea; t++)
                {
                    out_f = 0.0;
                    for (size_t s = 0; s < GP.SITES; s++)
                    {
                        if (strncmp(GP.VARNAME, "TEM", 3) == 0)
                        {
                            // TEM_AVG, TEM_MAX, TEM_MIN
                            out_f +=
                                tem_correct(*(rdata + t * GP.SITES + s), (coor + s)->DEM, cell_dem) *
                                *(weight + i * GEO_header.ncols * GP.SITES + j * GP.SITES + s);
                        }
                        else
                        {
                            // WIN, PRS, PRE, SSD
                            out_f +=
                                *(rdata + t * GP.SITES + s) *
                                *(weight + i * GEO_header.ncols * GP.SITES + j * GP.SITES + s);
                        }
                    }
                    out_int = (int)out_f;
                    *(rdata_out + t * GEO_header.ncols * GEO_header.nrows + i * GEO_header.ncols + j) = out_int;
                }
            }
            // printf("row: %3d ncol: %3d\n", i, j);
        }
    }
    nc_close(ncID);

    char att_longname[100];
    char att_unit[50];
    double scale_factor;
    if (strcmp(GP.VARNAME, "PRE") == 0)
    {
        strcpy(att_longname, "precipitation");
        strcpy(att_unit, "mm");
        scale_factor = 0.1;
    } else if (strcmp(GP.VARNAME, "PRS") == 0)
    {
        strcpy(att_longname, "atmospheric pressure");
        strcpy(att_unit, "kPa");
        scale_factor = 1.0;
    } else if (strcmp(GP.VARNAME, "WIN") == 0)
    {
        strcpy(att_longname, "wind speed");
        strcpy(att_unit, "m/s");
        scale_factor = 0.1;
    } else if (strcmp(GP.VARNAME, "SSD") == 0)
    {
        strcpy(att_longname, "sunshine duration");
        strcpy(att_unit, "hours");
        scale_factor = 1;
    } else if (strcmp(GP.VARNAME, "RHU") == 0)
    {
        strcpy(att_longname, "relative humidity");
        strcpy(att_unit, "%");
        scale_factor = 1.0;
    } else if (strcmp(GP.VARNAME, "TEM_AVG") == 0)
    {
        strcpy(att_longname, "average air temperature");
        strcpy(att_unit, "Celsius Degree");
        scale_factor = 0.1;
    } else if (strcmp(GP.VARNAME, "TEM_MAX") == 0)
    {
        strcpy(att_longname, "maximum air temperature");
        strcpy(att_unit, "Celsius Degree");
        scale_factor = 0.1;
    } else if (strcmp(GP.VARNAME, "TEM_MIN") == 0)
    {
        strcpy(att_longname, "minimum air temperature");
        strcpy(att_unit, "Celsius Degree");
        scale_factor = 0.1;
    }
    // printf("VARNAME: %10s\nunit: %30s\n", GP.VARNAME, att_unit);
    

    Write2NC(
        GP.VARNAME,
        att_unit,
        att_longname,
        scale_factor,
        GP.FP_GEO,
        GP.FP_OUT,
        &rdata_out,
        GP.STEP_TIME,
        nrows_wea,
        GP.START_YEAR, GP.START_MONTH,
        GP.START_DAY, GP.START_HOUR
        );
    printf("write %s data into NetCDF file: Done!\n", GP.VARNAME);
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

double tem_correct(
    int air_tem_donor,
    double altitude_donor,
    double altitude_target
)
{
    /***********
     * air temperature elapse rate with altitude:
     * 0.65 celsius degree per 100m;
     * here, the unit of air_tem_donor is: 0.1C;
    */
    double weight;
    double air_tem_correct;
    air_tem_correct = (double)air_tem_donor + (altitude_donor - altitude_target) / 100 * 0.65 * 10;
    return air_tem_correct;
}
