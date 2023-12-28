#ifndef NETCDF_IO_GEO
#define NETCDF_IO_GEO

#include <time.h>

void Write2NC(
    char varNAME[],
    char att_unit[],
    char att_longname[],
    char FP_GEO[],
    char FP_output[],
    double **rdata,
    time_t start_time,
    int step_time,
    int ts_length
);

#endif
