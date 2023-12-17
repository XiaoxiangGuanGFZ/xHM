
#include <stdio.h>
#include <stdlib.h>
#include <netcdf.h>

#include "GEO_ST.h"
#include "Route_UH.h"

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return e;}
int copy_global_attributes(int input_ncid, int output_ncid);

void main()
{
    int i,j;
    int ncID_GEO;
    char FP_GEO[MAXCHAR] = "D:/xHM/example_data/JN_GEO_1km/GEO_data.nc";
    ST_Header GEO_header;
    int varID_FDR, varID_FAC, varID_DEM;
    int *data_DEM;
    int *data_FDR;
    int *data_FAC;
    int cellsize_m;
    nc_open(FP_GEO, NC_WRITE, &ncID_GEO);
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "ncols", &GEO_header.ncols);
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "nrows", &GEO_header.nrows);
    nc_get_att_int(ncID_GEO, NC_GLOBAL, "cellsize_m", &cellsize_m);
    printf("ncols: %d\nnrows: %d\n", GEO_header.ncols, GEO_header.nrows);

    nc_inq_varid(ncID_GEO, "DEM", &varID_DEM);
    nc_inq_varid(ncID_GEO, "FDR", &varID_FDR);
    nc_inq_varid(ncID_GEO, "FAC", &varID_FAC);
    data_DEM = (int *)malloc(sizeof(int) * GEO_header.nrows * GEO_header.ncols);
    data_FDR = (int *)malloc(sizeof(int) * GEO_header.nrows * GEO_header.ncols);
    data_FAC = (int *)malloc(sizeof(int) * GEO_header.nrows * GEO_header.ncols);
    
    nc_get_var_int(ncID_GEO, varID_DEM, data_DEM);
    nc_get_var_int(ncID_GEO, varID_FDR, data_FDR);
    nc_get_var_int(ncID_GEO, varID_FAC, data_FAC);

    nc_get_att_int(ncID_GEO, varID_DEM, "NODATA_value", &GEO_header.NODATA_value);

    /****** UH parameters ***/
    double *data_Slope;
    double *data_FlowDistance;
    data_Slope = (double *)malloc(sizeof(double) * GEO_header.nrows * GEO_header.ncols);
    data_FlowDistance = (double *)malloc(sizeof(double) * GEO_header.nrows * GEO_header.ncols);
    Grid_Slope(data_DEM, data_FDR, data_Slope, data_FlowDistance,
               GEO_header.ncols,
               GEO_header.nrows,
               GEO_header.NODATA_value,
               cellsize_m);
    
    double V_avg = 480.0;
    double V_max = 13200.0;
    double V_min = 100.0;
    double b = 0.5;
    double c = 0.5;
    double *data_SlopeArea;
    data_SlopeArea = (double *)malloc(sizeof(double) * GEO_header.nrows * GEO_header.ncols);
    double slope_area_avg;
    Grid_SlopeArea(data_FAC, data_Slope, data_SlopeArea,
                   &slope_area_avg, b, c,
                   GEO_header.ncols,
                   GEO_header.nrows,
                   GEO_header.NODATA_value,
                   cellsize_m);
    printf("average of slope_area term: %.3f\n", slope_area_avg);

    double *data_V;
    data_V = (double *)malloc(sizeof(double) * GEO_header.nrows * GEO_header.ncols);
    Grid_Velocity(data_FAC, data_SlopeArea, slope_area_avg, data_V,
                  V_avg, V_max, V_min,
                  GEO_header.ncols,
                  GEO_header.nrows,
                  GEO_header.NODATA_value);
    
    double *data_FlowTime;
    data_FlowTime = (double *)malloc(sizeof(double) * GEO_header.nrows * GEO_header.ncols);

    Grid_FlowTime(data_DEM, data_FDR, data_V, data_FlowDistance, data_FlowTime,
                  16, // int outlet_index_row,
                  39, // int outlet_index_col,
                  GEO_header.ncols,
                  GEO_header.nrows,
                  GEO_header.NODATA_value);
    
    double *data_UH;
    int time_steps;
    int step_time = 24;  // 1 hour
    double beta = 0.5;
    Grid_UH(data_DEM, data_FlowTime, &data_UH,
            &time_steps, beta, step_time,
            GEO_header.ncols,
            GEO_header.nrows,
            GEO_header.NODATA_value);
    
    printf("total time steps in UH: %d\n", time_steps);
    /********** to NetCDF ***********/
    int ncID_UH;
    int dimID_lon, dimID_lat, dimID_time;
    int varID_UH, varID_Slope, varID_FlowDistance, varID_SlopeArea, varID_V, varID_FlowTime; // varID_lon, varID_lat,
    nc_create("D:/JN_UH.nc", NC_CLOBBER, &ncID_UH);
    
    nc_def_dim(ncID_UH, "lon", GEO_header.ncols, &dimID_lon);
    nc_def_dim(ncID_UH, "lat", GEO_header.nrows, &dimID_lat);
    nc_def_dim(ncID_UH, "time", NC_UNLIMITED, &dimID_time);
    int dims[3];
    dims[0] = dimID_time;
    dims[1] = dimID_lat;
    dims[2] = dimID_lon;
    
    // nc_def_var(ncID_UH, "lon", NC_DOUBLE, 1, &dimID_lon, &varID_lon);
    // nc_def_var(ncID_UH, "lat", NC_DOUBLE, 1, &dimID_lat, &varID_lat);
    // nc_def_var(ncID_UH, "time", NC_INT, 1, &dimID_time, &varID_time);
    nc_def_var(ncID_UH, "Slope", NC_DOUBLE, 2, dims + 1, &varID_Slope);
    nc_def_var(ncID_UH, "FlowDistance", NC_DOUBLE, 2, dims + 1, &varID_FlowDistance);
    nc_def_var(ncID_UH, "SlopeArea", NC_DOUBLE, 2, dims + 1, &varID_SlopeArea);
    nc_def_var(ncID_UH, "Velocity", NC_DOUBLE, 2, dims + 1, &varID_V);
    nc_def_var(ncID_UH, "FlowTime", NC_DOUBLE, 2, dims + 1, &varID_FlowTime);
    nc_def_var(ncID_UH, "UH", NC_DOUBLE, 3, dims, &varID_UH);
    
    // nc_put_att_int(ncID_weather, varID_PRE, "NODATA_value", NC_INT, 1, &GEO_header.NODATA_value);

    copy_global_attributes(ncID_GEO, ncID_UH);
    
    nc_enddef(ncID_UH);
    nc_put_var_double(ncID_UH, varID_Slope, data_Slope);
    nc_put_var_double(ncID_UH, varID_FlowDistance, data_FlowDistance);
    nc_put_var_double(ncID_UH, varID_SlopeArea, data_SlopeArea);
    nc_put_var_double(ncID_UH, varID_V, data_V);   
    nc_put_var_double(ncID_UH, varID_FlowTime, data_FlowTime); 
    // nc_put_var_double(ncID_UH, varID_UH, data_UH); 
    size_t index[3] = {0,0,0};
    int t;
    for (t = 0; t < time_steps; t++)
    {
        index[0] = t;
        for (i = 0; i < GEO_header.nrows; i++)
        {
            index[1] = i;
            for (j = 0; j < GEO_header.ncols; j++)
            {
                index[2] = j;
                nc_put_var1_double(
                    ncID_UH, varID_UH, index,
                    data_UH + t * GEO_header.ncols * GEO_header.nrows + i * GEO_header.ncols + j);
            } 
        }    
        // printf("t: %d\n", t);    
    }
    
    nc_close(ncID_GEO);
    nc_close(ncID_UH);
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

