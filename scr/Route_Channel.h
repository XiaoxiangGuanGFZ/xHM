#ifndef ROUTE_CHANNEL
#define ROUTE_CHANNEL

void Channel_Routing(
    double *Qin,
    double *Qout,
    double *V,
    double Qc,
    double k,
    int step_time);

void Initialize_STREAM(
    CELL_VAR_STREAM **data_STREAM,
    int *data_STR,
    int *data_FDR,
    double ROUTE_CHANNEL_k,
    int NODATA_value,
    int ncols,
    int nrows);

void Channel_Network_Routing(
    CELL_VAR_STREAM **data_STREAM,
    int *data_STR,
    int ncols,
    int nrows,
    int step_time);

#endif