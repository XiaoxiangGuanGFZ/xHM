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


#endif