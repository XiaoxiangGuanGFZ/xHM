
#ifndef LOOKUP_SOILLIB
#define LOOKUP_SOILLIB

typedef struct 
{
    int Code;                /* texture code */
    char Texture[20];        /* USDA Texture */
    double WiltingPoint;     /* Wilting point (%Vol) */
    double FieldCapacity;    /* Field capacity (%Vol) */
    double Saturation;       /* Saturation (%Vol) */
    double Residual;         /* Residual content (%Vol) */
    double AvailWater;       /* Available water (cm/cm) */
    double SatHydrauCond;    /* Sat.Hydraulic Cond.(mm/hr) */
    double BulkDensity;      /* Matric Bulk Density(g/cm3) */
    double Porosity;         /* soil porosity (%Vol) */
    double PoreSizeDisP;     /* pore size distribution parameter,b */
    double AirEntryPresHead; /* air-entry pressure head (cm) */
    double Bubbling;         /* Bubbling pressure (cm) */

} ST_SoilLib;

typedef struct 
{
    int ID;                  /* HWSD Database ID */
    int T_SAND;              /* Topsoil Sand Frac (%) */
    int T_SILT;              /* Topsoil Silt Frac (%) */
    int T_CLAY;              /* Topsoil Clay Frac (%) */
    int T_USDA_TEX_CLASS;    /* Topsoil USDA Texture Classification */
    double T_OC;             /* Topsoil Organic Carbon (%) */
    int S_SAND;              /* Subsoil Sand Frac (%) */
    int S_SILT;              /* Subsoil Silt Frac (%) */
    int S_CLAY;              /* Subsoil Clay Frac (%) */
    int S_USDA_TEX_CLASS;    /* Subsoil USDA Texture Classification */
    double S_OC;             /* Subsoil Organic Carbon (%) */
} ST_SoilID;

typedef struct 
{
    int ID;
    int T_USDA_TEX_CLASS;
    int S_USDA_TEX_CLASS;
    ST_SoilLib *Topsoil; 
    ST_SoilLib *Subsoil;
} ST_SOIL_LIB_CELL;


void Import_soillib(
    char FP[],
    ST_SoilLib soillib[]
);

void Import_soil_HWSD_ID(
    char FP[],
    ST_SoilID soilID[]
);

void Lookup_Soil_ID(
    int ID,
    int *T_USDA_TEX_CLASS,
    int *S_USDA_TEX_CLASS,
    ST_SoilID soilID[]
);

void Lookup_Soil_CELL(
    int ID,
    ST_SOIL_LIB_CELL *soil_cell_para, 
    ST_SoilLib soillib[],
    ST_SoilID soilID[]
);

#endif