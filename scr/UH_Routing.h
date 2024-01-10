#ifndef UH_ROUTE
#define UH_ROUTE
#include "GEO_ST.h"
#include "UH_Generation.h"

void UH_Read(
    int ncID_UH,
    int *varID_UH,
    int *outlet_count,
    int *outlet_index_row,
    int *outlet_index_col,
    int *UH_steps
);

void UH_Import(
    int ncID_UH,
    int *varID_UH,
    int outlet_count,
    int cell_counts_total,
    int *UH_steps,
    double **data_UH
);

void UH_Routing(
    int *data_RUNOFF_sf,
    double *data_UH,
    double **Qout,
    int UH_steps,
    int ncols,
    int nrows,
    int time_steps_run,
    int cellsize_m,
    int NODATA_value,
    int STEP_TIME
);

int IsNODATA(
    double value,
    int NODATA_value
);

#endif