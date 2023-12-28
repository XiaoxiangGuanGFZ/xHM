#ifndef SOIL_STRUCTURE
#define SOIL_STRUCTURE

#include "Lookup_SoilLib.h"

typedef struct 
{
    int ID;
    int T_USDA_TEX_CLASS;
    int S_USDA_TEX_CLASS;
    ST_SoilLib *Topsoil; 
    ST_SoilLib *Subsoil;
} ST_SOIL_LIB_CELL;

#endif

