#ifndef NETCDF_IO_GEO
#define NETCDF_IO_GEO

#include <time.h>

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
);

#endif
