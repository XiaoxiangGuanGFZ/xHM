#ifndef HM_GLOBALPARA
#define HM_GLOBALPARA

#include "HM_ST.h"

void Import_GlobalPara(
    char FP[],
    GLOBAL_PARA *global_para
);

void Initialize_GlobalPara(
    GLOBAL_PARA *global_para
);

void Print_GlobalPara(
    GLOBAL_PARA *gp
);

#endif

