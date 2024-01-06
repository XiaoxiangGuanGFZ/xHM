
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
    int **out_Rs
);

#endif