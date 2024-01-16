#ifndef ROUTE_CHANNEL
#define ROUTE_CHANNEL

// void Channel_Routing(
//     double *Qin,
//     double *Qout,
//     double *Vin,
//     double *Vout,
//     double k,
//     int step_time
// );

void Channel_Routing(
    double *Q,
    double *V,
    double k,
    int step_time
);

void Initialize_STREAM(
    CELL_VAR_STREAM *data_STREAM,
    int *data_STR,
    int NODATA_value,
    int ncols,
    int nrows);


#endif