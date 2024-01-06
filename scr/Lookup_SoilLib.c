/*
 * SUMMARY:      Lookup_SoilLib.c
 * USAGE:        look up the soil properties from soil_lib
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  given a soil type from a cell (ID, from HWSD raster), 
 *               derive the full set parameters for the grid cell, 
 * DESCRIP-END.
 * FUNCTIONS:    Import_soillib(); Import_soil_HWSD_ID(); Lookup_Soil_ID(); 
 *               Lookup_Soil_CELL()
 * 
 * COMMENTS:
 * 
 *
 */

/*******************************************************************************
 * VARIABLEs:
 * char FP[]                 - file path and file name 
 * 
 * 
 * 
********************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Constants.h"
#include "Lookup_SoilLib.h"


void Import_soillib(
    char FP[],
    ST_SoilLib soillib[])
{
    /**********************
     * read in the soil property (parameters) library
     * from SOIL_LIB.txt in to a structure array soillib[]
     * with the size of 13;
     */
    FILE *fp;
    if ((fp = fopen(FP, "r")) == NULL)
    {
        printf("cannot open SOIL_LIB file %s\n", FP);
        exit(0);
    }
    char row[MAXCHAR];
    int i;
    int j = 0;
    while (fgets(row, MAXCHAR, fp) != NULL)
    {
        if (row != NULL && strlen(row) > 1 && row[0] != '#')
        {
            for (i = 0; i < strlen(row); i++)
            {
                /* remove all the characters after # */
                if (row[i] == '#')
                {
                    row[i] = '\0'; // replace the '#' with '\0', end sign.
                }
            }
            (soillib + j)->Code = atoi(strtok(row, "\t"));
            strcpy((soillib + j)->Texture, strtok(NULL, "\t"));
            (soillib + j)->WiltingPoint = atof(strtok(NULL, "\t"));
            (soillib + j)->FieldCapacity = atof(strtok(NULL, "\t"));
            (soillib + j)->Saturation = atof(strtok(NULL, "\t"));
            (soillib + j)->Residual = atof(strtok(NULL, "\t"));
            (soillib + j)->AvailWater = atof(strtok(NULL, "\t"));
            (soillib + j)->SatHydrauCond = atof(strtok(NULL, "\t"));
            (soillib + j)->BulkDensity = atof(strtok(NULL, "\t"));
            (soillib + j)->Porosity = atof(strtok(NULL, "\t"));
            (soillib + j)->PoreSizeDisP = atof(strtok(NULL, "\t"));
            (soillib + j)->AirEntryPresHead = atof(strtok(NULL, "\t"));
            (soillib + j)->Bubbling = atof(strtok(NULL, "\t"));
            j++;
        }
    }
}

void Import_soil_HWSD_ID(
    char FP[],
    ST_SoilID soilID[]
)
{
    /***************************
     * import the HWSD ID data
     * The (unique) ID field corresponds to the 2D raster values;
     * size of array soilID[]: 1000
    */    
    FILE *fp;
    if ((fp = fopen(FP, "r")) == NULL)
    {
        printf("cannot open SOIL_HWSD_ID.txt file %s\n", FP);
        exit(0);
    }
    char row[MAXCHAR];
    int i;
    int j = 0;
    while (fgets(row, MAXCHAR, fp) != NULL)
    {
        if (row != NULL && strlen(row) > 1 && row[0] != '#')
        {
            for (i = 0; i < strlen(row); i++)
            {
                /* remove all the characters after # */
                if (row[i] == '#')
                {
                    row[i] = '\0'; // replace the '#' with '\0', end sign.
                }
            }
            (soilID + j)->ID = atoi(strtok(row, "\t"));
            (soilID + j)->T_SAND = atoi(strtok(NULL, "\t"));
            (soilID + j)->T_SILT = atoi(strtok(NULL, "\t"));
            (soilID + j)->T_CLAY = atoi(strtok(NULL, "\t"));
            (soilID + j)->T_USDA_TEX_CLASS = atoi(strtok(NULL, "\t"));
            (soilID + j)->T_OC = atof(strtok(NULL, "\t"));
            (soilID + j)->S_SAND = atoi(strtok(NULL, "\t"));
            (soilID + j)->S_SILT = atoi(strtok(NULL, "\t"));
            (soilID + j)->S_CLAY = atoi(strtok(NULL, "\t"));
            (soilID + j)->S_USDA_TEX_CLASS = atoi(strtok(NULL, "\t"));
            (soilID + j)->S_OC = atof(strtok(NULL, "\t"));
            j++;
        }
    }
}

void Lookup_Soil_ID(
    int ID,
    int *T_USDA_TEX_CLASS,
    int *S_USDA_TEX_CLASS,
    ST_SoilID soilID[]
)
{
    int index=0;
    index = ID - 11000;  // the HWSD ID starts from 11000;
    *T_USDA_TEX_CLASS = (soilID + index)->T_USDA_TEX_CLASS;
    *S_USDA_TEX_CLASS = (soilID + index)->S_USDA_TEX_CLASS;
    if (*T_USDA_TEX_CLASS == -99)
    {
        printf("we didn't find the USDA_TEX_CLASS for the soil type (ID): %d ",
            ID
        );
        exit(-1);
    } else if (*S_USDA_TEX_CLASS == -99.9)
    {
        *S_USDA_TEX_CLASS = *T_USDA_TEX_CLASS;
    }
}

void Lookup_Soil_CELL(
    int ID,
    ST_SOIL_LIB_CELL *soil_cell_para, 
    ST_SoilLib soillib[],
    ST_SoilID soilID[]
)
{
    /**************
     * each grid cell has a HWSD ID, which is associated 
     * with the full-set soil properties (parameters) for 
     * two layers: topsoil and subsoil
     * 
    */
    int T_USDA_TEX_CLASS, S_USDA_TEX_CLASS;
    Lookup_Soil_ID(ID, &T_USDA_TEX_CLASS, &S_USDA_TEX_CLASS, &(*soilID));
    //printf("T_CLASS: %d\nS_CLASS: %d\n", T_USDA_TEX_CLASS, S_USDA_TEX_CLASS);
    soil_cell_para->ID = ID;
    soil_cell_para->S_USDA_TEX_CLASS = S_USDA_TEX_CLASS;
    soil_cell_para->T_USDA_TEX_CLASS = T_USDA_TEX_CLASS;
    soil_cell_para->Topsoil = &(soillib[T_USDA_TEX_CLASS - 1]);
    soil_cell_para->Subsoil = &(soillib[S_USDA_TEX_CLASS - 1]);
}


// void main()
// {
//     char FP_SOILLIB[] = "D:/xHM/example_data/SOIL_LIB.txt";
//     ST_SoilLib soillib[13];
//     Import_soillib(FP_SOILLIB, soillib);
//     printf("%10s%20s%10s%10s%10s\n",
//         "Code", "Texture", "Residual", "Porosity", "Bubbling"
//     );
//     for (size_t i = 0; i < 13; i++)
//     {
//         printf("%10d%20s%10.1f%10.1f%10.1f\n",
//             (soillib + i)->Code,
//             (soillib + i)->Texture,
//             (soillib + i)->Residual,
//             (soillib + i)->Porosity,
//             (soillib + i)->Bubbling
//         );
//     }
    
//     char FP_SOILID[] = "D:/xHM/example_data/SOIL_HWSD_ID.txt";
//     ST_SoilID soilID[1000];
//     Import_soil_HWSD_ID(FP_SOILID, soilID);
//     printf("%10s%10s%10s%10s%10s\n",
//         "ID", "T_SAND", "T_SILT", "T_CLAY", "T_USDA_TEX_CLASS"
//     );
//     for (size_t i = 0; i < 5; i++)
//     {
//         printf("%10d%10d%10d%10d%10d\n",
//             (soilID + i)->ID,
//             (soilID + i)->T_SAND,
//             (soilID + i)->T_SILT,
//             (soilID + i)->T_CLAY,
//             (soilID + i)->T_USDA_TEX_CLASS
//         );
//     }
//     // ----- 
//     int ID;
//     ID = 11010;
//     ST_SOIL_LIB_CELL soil_cell_para;
//     printf("ID: %d\n", ID);
//     Lookup_Soil_CELL(ID, &soil_cell_para, soillib, soilID);
//     printf("%10s%20s%10s%10s%10s\n",
//         "Code", "Texture", "Residual", "Porosity", "Bubbling"
//     );
//     printf("%10d%20s%10.1f%10.1f%10.1f\n",
//            (soil_cell_para.Topsoil)->Code,
//            (soil_cell_para.Topsoil)->Texture,
//            (soil_cell_para.Topsoil)->Residual,
//            (soil_cell_para.Topsoil)->Porosity,
//            (soil_cell_para.Topsoil)->Bubbling);
// }
