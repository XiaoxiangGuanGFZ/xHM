
#ifndef ROUTE_UH
#define ROUTE_UH

#include "GEO_ST.h"
#define MAX_OUTLETS 100     // maximum number of outlets

void Grid_Slope(
    int *data_DEM,
    int *data_FDR,
    double *data_Slope,
    double *data_FlowDistance,
    int ncols,
    int nrows,
    int NODATA_value,
    int cellsize_m
);

void Grid_SlopeArea(
    int *data_FAC,
    double *data_Slope,
    double *data_SlopeArea,
    double *slope_area_avg,
    double b,
    double c,
    int ncols,
    int nrows,
    int NODATA_value,
    int cellsize_m
);

void Grid_Velocity(
    int *data_FAC,
    double *data_SlopeArea,
    double slope_area_avg,
    double *data_V,
    double V_avg,
    double V_max,
    double V_min,
    int ncols,
    int nrows,
    int NODATA_value
);

void Grid_Outlets(
    int *data_Outlet,
    int outlet_index_row[],
    int outlet_index_col[],
    int *outlet_count,
    int ncols,
    int nrows,
    int NODATA_value
);

void Grid_OutletMask(
    int outlet_index_row,
    int outlet_index_col,
    int *data_FDR,
    int *data_Mask,
    int ncols,
    int nrows,
    int NODATA_value
);


void Grid_FlowTime(
    int *data_mask,
    int *data_FDR,
    double *data_V,
    double *data_FlowDistance,
    double *data_FlowTime,
    int outlet_index_row,
    int outlet_index_col,
    int ncols,
    int nrows,
    int NODATA_value
);

void Grid_UH(
    int *data_mask,
    double *data_FlowTime,
    double **data_UH,
    int *time_steps,
    double beta,
    int step_time,
    int ncols,
    int nrows,
    int NODATA_value
);

void UH_Generation(
    char FP_GEO[],
    char FP_UH[],
    int step_time,
    double Velocity_avg,
    double Velocity_max, 
    double Velocity_min,
    double b, 
    double c
);

int FloatEqual(
    double x1,
    double x2);

#endif


