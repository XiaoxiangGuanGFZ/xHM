
#include <stdio.h>
#include <netcdf.h>
#include <stdlib.h>

void handle_error(int status);
void print2D(
    double *ptr,
    int m,
    int n
);

void main()
{
    int ncID;
    int status;
    int dimID_lon, dimID_lat;

    int varID_data;
    int varID_lon, varID_lat;
    int old_fill_mode;
    
    int data_dims[2];
    
    // data
    double data_lon[3] = {126.25, 127.25, 128.25};
    double data_lat[2] = {27.0, 28.0};
    double data[3][2] = {
        {200, 201.5},
        {198.5, 196.2},
        {184.5, 200.0}
    };
    print2D(&data[0][0], 3, 2);
    // double data[4] = {200, 215.0, 195.5, 189.2};

    status = nc_create("./ncdata.nc", NC_CLOBBER, &ncID);
    if (status != NC_NOERR) handle_error(status);

    // def dimension
    nc_def_dim(ncID, "lon", 3L, &dimID_lon);
    nc_def_dim(ncID, "lat", 2L, &dimID_lat);
    // new variables
    data_dims[0] = dimID_lon;
    data_dims[1] = dimID_lat;
    nc_def_var(ncID, "data", NC_DOUBLE, 2, data_dims, &varID_data);
    nc_def_var(ncID, "lon", NC_DOUBLE, 1, &dimID_lon, &varID_lon);
    nc_def_var(ncID, "lat", NC_DOUBLE, 1, &dimID_lat, &varID_lat);

    nc_set_fill(ncID, NC_FILL, &old_fill_mode);
    nc_enddef(ncID);
    
    // data put
    size_t start[2] = {0,0};
    size_t counts[2] = {3,2};
    ptrdiff_t stride[2] = {1,1};
    ptrdiff_t imap[2] = {2, 1};
    // nc_put_var_double(ncID, varID_data, data);
    nc_put_var_double(ncID, varID_lon, data_lon);
    nc_put_var_double(ncID, varID_lat, data_lat);
    nc_put_varm_double(ncID, varID_data, start, counts, stride, imap, &data[0][0]);
    nc_close(ncID);
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
    double *ptr,
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

