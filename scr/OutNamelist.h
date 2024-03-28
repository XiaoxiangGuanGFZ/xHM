
#ifndef OUTNAMELIST
#define OUTNAMELIST
#include "GEO_ST.h"
void Import_Outnamelist(
    char FP[],
    OUT_NAME_LIST *outnl
);

void Initialize_Outnamelist(
    OUT_NAME_LIST *outnl
);

void malloc_Outnamelist(
    OUT_NAME_LIST outnl,
    OUT_NAME_LIST *outnl_ncid,
    int cell_counts_total,
    int time_steps_run,
    int **data_DEM,
    ST_Header HD,
    GLOBAL_PARA GP,
    int **out_Rs,
    int **out_L_sky,
    int **out_Rno,
    int **out_Rnu,
    int **out_Ep,
    int **out_EI_o,
    int **out_EI_u,
    int **out_ET_o,
    int **out_ET_u,
    int **out_ET_s,
    int **out_Interception_o,
    int **out_Interception_u,
    int **out_Prec_net,
    int **out_SM_Upper,
    int **out_SM_Lower,
    int **out_SW_Percolation_Upper,
    int **out_SW_Percolation_Lower,
    int **out_SW_Infiltration,
    int **out_SW_SUB_Qin, 
    int **out_SW_SUB_Qout, 
    int **out_SW_SUB_z, 
    int **out_SW_SUB_rise_upper, 
    int **out_SW_SUB_rise_lower, 
    int **out_SW_SUB_rf,
    int **out_SW_SUB_Qc,
    int **out_Q_Channel
);

void OUTVAR_nc_initial(
    int **data_DEM,
    int **out_data,
    ST_Header HD);

int OUTVAR_nc_create(
    char varNAME[],
    char att_unit[],
    char att_longname[],
    double scale_factor,
    char FP_output[],
    int ts_length,
    GLOBAL_PARA GP);

void Write_Outnamelist(
    int t_run,
    OUT_NAME_LIST outnl,
    OUT_NAME_LIST outnl_ncid,
    ST_Header HD,
    int **out_Rs,
    int **out_L_sky,
    int **out_Rno,
    int **out_Rnu,
    int **out_Ep,
    int **out_EI_o,
    int **out_EI_u,
    int **out_ET_o,
    int **out_ET_u,
    int **out_ET_s,
    int **out_Interception_o,
    int **out_Interception_u,
    int **out_Prec_net,
    int **out_SM_Upper,
    int **out_SM_Lower,
    int **out_SW_Percolation_Upper,
    int **out_SW_Percolation_Lower,
    int **out_SW_Infiltration,
    int **out_SW_SUB_Qin, 
    int **out_SW_SUB_Qout, 
    int **out_SW_SUB_z, 
    int **out_SW_SUB_rise_upper, 
    int **out_SW_SUB_rise_lower, 
    int **out_SW_SUB_rf,
    int **out_SW_SUB_Qc,
    int **out_Q_Channel
);

void OUTVAR_nc_write(
    int ncID,
    char varNAME[],
    int **out_data,
    int index_start[3],
    int index_count[3]
);

void OUTVAR_nc_close(
    OUT_NAME_LIST outnl,
    OUT_NAME_LIST outnl_ncid);

void Write2NC_Outnamelist(
    OUT_NAME_LIST outnl,
    int time_steps_run,
    int **out_SW_Run_Infil,
    int **out_SW_Run_Satur,
    GLOBAL_PARA GP
);

void malloc_memory_error(
    int *data,
    char var[]
);

#endif