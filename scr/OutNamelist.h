
#ifndef OUTNAMELIST
#define OUTNAMELIST

void Import_Outnamelist(
    char FP[],
    OUT_NAME_LIST *outnl
);

void Initialize_Outnamelist(
    OUT_NAME_LIST *outnl
);

void malloc_Outnamelist(
    OUT_NAME_LIST outnl,
    int cell_counts_total,
    int time_steps_run,
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
    int **out_SW_Run_Infil,
    int **out_SW_Run_Satur,
    int **out_SW_SUB_Qin, 
    int **out_SW_SUB_Qout, 
    int **out_SW_SUB_z, 
    int **out_SW_SUB_rise_upper, 
    int **out_SW_SUB_rise_lower, 
    int **out_SW_SUB_rf
);

void malloc_memory_error(
    int *data,
    char var[]
);

void Write2NC_Outnamelist(
    OUT_NAME_LIST outnl,
    int time_steps_run,
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
    int **out_SW_Run_Infil,
    int **out_SW_Run_Satur,
    int **out_SW_SUB_Qin, 
    int **out_SW_SUB_Qout, 
    int **out_SW_SUB_z, 
    int **out_SW_SUB_rise_upper, 
    int **out_SW_SUB_rise_lower, 
    int **out_SW_SUB_rf,
    GLOBAL_PARA GP
);

#endif