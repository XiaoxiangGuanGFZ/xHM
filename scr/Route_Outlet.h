#ifndef ROUTE_OUTLET
#define ROUTE_OUTLET

void Route_Outlet(
    double *Qout_SF_Infil,
    double *Qout_SF_Satur,
    double *Qout_Sub,
    double *Qout_outlet,
    int outlet_count,
    int time_steps_run
);


void Write_Qout(
    OUT_NAME_LIST outnl,
    char PATH_OUT[],
    double *Qout_SF_Infil,
    double *Qout_SF_Satur,
    double *Qout_Sub,
    double *Qout_outlet,
    int *outlet_index_row,
    int *outlet_index_col,
    int outlet_count,
    int time_steps_run
);


#endif