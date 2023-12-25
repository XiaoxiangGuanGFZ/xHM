
#ifndef Lookup_VegLib
#define Lookup_VegLib

typedef struct 
{
    int ID;                     /* vegetation class / ID*/
    int Overstory;              /* overstory: whether there is an overstory; yes:1; no:0*/
    double Rarc;                /* Architectural resistance of vegetation type (~2 s/m) */
    double Rmin;                /* Minimum stomatal resistance of vegetation type (~100 s/m) */
    double LAI[12];             /* Leaf-area index of vegetation type, fraction (m2/m2) */
    double Albedo[12];          /* Shortwave albedo for vegetation type */
    double Displacement[12];    /* Vegetation displacement height (typically 0.67 * vegetation height) */
    double Roughness[12];       /* Vegetation roughness length (typically 0.123 * vegetation height) */
    double SAI;                 /* Stem area index (SAI), fraction */
    double CAN_TOP;             /* height of canopy top, [m] */
    double CAN_BOTT;            /* height of canopy bottom, [m] */
    double WIND_H;              /* Height at which wind speed is measured. */
    double RGL;                 /* Minimum incoming shortwave radiation at which there will be transpiration. For trees this is about 30 W/m2, for crops about 100 W/m2. */
    double SolAtn;              /* Radiation attenuation factor. Normally set to 0.5, though may need to be adjusted for high latitudes. */
    double WndAtn;              /* Wind speed attenuation through the overstory. The default value has been 0.5. */
    double Trunk;               /* Ratio of total tree height that is trunk (no branches). The default value has been 0.2. */
} ST_VegLib;

void Import_veglib(
    char FP[],
    ST_VegLib veglib[]
);

void LookUp_veglib(
    ST_VegLib veglib[],
    int ID,
    int month,
    int *Overstory,
    double *Rarc,
    double *Rmin,
    double *LAI,
    double *Roughness,
    double *Displacement,
    double *Albedo,
    double *SAI,
    double *CAN_TOP,
    double *CAN_BOTT,
    double *WIND_H
);

void Lookup_VegLib_TYPE(
    ST_VegLib veglib[],
    int CLASS,
    double *Rmin,
    double *CAN_TOP,
    double *WIND_H
);

void Lookup_VegLib_CELL(
    ST_VegLib veglib[],
    int CLASS,
    ST_CELL_VEG *cell_veg
);

void Lookup_VegLib_CELL_MON(
    ST_VegLib veglib[],
    int CLASS,
    int month,
    ST_CELL_VEG *cell_veg
);

#endif
