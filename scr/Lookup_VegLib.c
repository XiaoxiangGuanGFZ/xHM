/*
 * SUMMARY:      Lookup_VegLib.c
 * USAGE:        derive the vegetation parameters 
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  Calculate evapotranspiration
 * DESCRIP-END.
 * FUNCTIONS:    Import_veglib(), LookUp_veglib()
 *                  
 * 
 * COMMENTS:
 * - derive the vegetation parameters based on vegetation type and seasonality (month)
 * - the vegetation library (VegeLib.txt) is universal for diffeent regions 
 * 
 * REFERENCES:
 * bare ground parameters from source: 
 *      "Mapped Static Vegetation Data" NASA Land Data Assimilation Systems
 */

/*******************************************************************************
 * VARIABLEs:
 * the description on variables in vegetation parameter library 
 * can be found in the struction definition in "Lookup_VegLib.h"
 * 
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Constants.h"
#include "Evapotranspiration_ST.h"
#include "Lookup_VegLib.h"

void Import_veglib(
    char FP[],
    ST_VegLib veglib[]
)
{
    /**********************
     * read in the vegetation parameter library
     * from VegeLib.txt in to a structure array veglib[]
     * with the size of 11;
    */
    FILE *fp;
    if((fp = fopen(FP, "r")) == NULL)
    {
        printf("cannot open file %s\n", FP);
        exit(0);
    }
    char row[MAXCHAR];
    int i;
    int j=0;
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
            (veglib + j)->ID = atoi(strtok(row, "\t"));
            (veglib + j)->Overstory = atoi(strtok(NULL, "\t"));
            (veglib + j)->Rarc = atof(strtok(NULL, "\t"));
            (veglib + j)->Rmin = atof(strtok(NULL, "\t"));
            for (size_t t = 0; t < 12; t++)
            {
                *(((veglib + j)->LAI) + t) = atof(strtok(NULL, "\t"));
            }
            for (size_t t = 0; t < 12; t++)
            {
                *(((veglib + j)->Albedo) + t) = atof(strtok(NULL, "\t"));                
            }
            for (size_t t = 0; t < 12; t++)
            {
                *(((veglib + j)->Roughness) + t) = atof(strtok(NULL, "\t"));                
            }
            for (size_t t = 0; t < 12; t++)
            {
                *(((veglib + j)->Displacement) + t) = atof(strtok(NULL, "\t"));                
            }
            (veglib + j)->SAI = atof(strtok(NULL, "\t"));
            (veglib + j)->CAN_TOP = atof(strtok(NULL, "\t"));
            (veglib + j)->CAN_BOTT = atof(strtok(NULL, "\t"));
            (veglib + j)->WIND_H = atof(strtok(NULL, "\t"));
            (veglib + j)->RGL = atof(strtok(NULL, "\t"));
            (veglib + j)->SolAtn = atof(strtok(NULL, "\t"));
            (veglib + j)->WndAtn = atof(strtok(NULL, "\t"));
            (veglib + j)->Trunk = atof(strtok(NULL, "\t"));
            j++;
        }
    }
}

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
)
{
    ST_VegLib *cell;
    cell = veglib + ID - 1;
    *Overstory = cell->Overstory;
    *Rarc = cell->Rarc;
    *Rmin = cell->Rmin;
    *LAI = cell->LAI[month - 1];
    *Roughness = cell->Roughness[month - 1];
    *Albedo = cell->Albedo[month - 1];
    *Displacement = cell->Displacement[month - 1];
    *SAI = cell->SAI;
    *CAN_TOP = cell->CAN_TOP;
    *CAN_BOTT = cell->CAN_BOTT;
    *WIND_H = cell->WIND_H;
    printf(
        "%10s%10s%10s%10s%10s%10s%10s%10s%10s%10s%10s%10s%10s\n",
        "ID", "month", "Overstory", "Rarc", "Rmin", 
        "LAI","Roughness", "Disp", "albedo",
        "SAI", "CAN_TOP", "CAN_BOTT", "WIND_H" 
    );
    printf(
        "%10d%10d%10d%10.3f%10.3f%10.3f%10.3f%10.3f%10.3f%10.3f%10.3f%10.3f%10.3f\n",
        ID, month, *Overstory, *Rarc, *Rmin, 
        *LAI, *Roughness, *Displacement, *Albedo,
        *SAI, *CAN_TOP, *CAN_BOTT, *WIND_H
    );
}

void Lookup_VegLib_TYPE(
    ST_VegLib veglib[],
    int CLASS,
    double *Rmin,
    double *CAN_TOP,
    double *WIND_H
)
{
    /*****
     * retrieve 3 veg parameter
    */
    ST_VegLib *cell;
    cell = veglib + CLASS - 1;
    *Rmin = cell->Rmin;
    *CAN_TOP = cell->CAN_TOP;
    *WIND_H = cell->WIND_H;
}

void Lookup_VegLib_CELL(
    ST_VegLib veglib[],
    int CLASS,
    ST_CELL_VEG *cell_veg
)
{
    /************************
     * retrieve veg parameters to a certain cell:
     * the veg parameter lib for a given cell is represented  
     *      as a structure (ST_CELL_VEG *cell_veg), 
     *      see "Evapotranspiration_ET.h". 
     * 
     * Pparameters obtained: 
     * - Rpc
     * - rs_min,: [s/m]
     * - CAN_H: canopy height [m]
     * - CAN_RZ: reference height (or wind speed observation height) of canopy [m]
     * 
     * attention: the parameters are constant through seasons
    */
    double BUF1, BUF2;
    cell_veg->Rpc = 30.0; // same for veg types, [W/m2];
    if (CLASS >=1 && CLASS <= 6)
    {
        // canopy (overstory == 1), both overstory and understory
        cell_veg->Understory = 1; 
        Lookup_VegLib_TYPE(
            veglib, CLASS, 
            &(cell_veg->rs_min_o),
            &(cell_veg->CAN_H),
            &(cell_veg->CAN_RZ)
        );
        
        // just assume the understory as grassland (CLASS == 10)
        Lookup_VegLib_TYPE(
            veglib, 10, 
            &(cell_veg->rs_min_u),
            &BUF1,
            &BUF2
            /* the understory height or wind speed observation height not involved
            */ 
        );
    }
    else if (CLASS > 6 && CLASS <= 11)
    {
        // no overstory, only understory
        cell_veg->Understory = 1; 
        cell_veg->rs_min_o = 0.0;
        cell_veg->CAN_H = 0.0;
        cell_veg->CAN_RZ = 0.0;
        Lookup_VegLib_TYPE(
            veglib, 10, 
            &(cell_veg->rs_min_u),
            &(cell_veg->CAN_H),
            &(cell_veg->CAN_RZ)
        );
    } 
    else if (CLASS == 0)
    {
        // actually this cell is open water
        // the model assumes it as bare soil, at present
        cell_veg->Understory = 0; 
        cell_veg->z0_u = 0.0012;
        cell_veg->d_u = 0.0;
    }
    else
    {
        printf("Recognized Vegetation type. Program failled\n");
        exit(0);
    }
        
}

void Lookup_VegLib_CELL_MON(
    ST_VegLib veglib[],
    int CLASS,
    int month,
    ST_CELL_VEG *cell_veg
)
{
    /*******
     * retrieve veg parameters to a certain grid cell
     * 
     * parameters:
     * - albedo
     * - displacement height (d) 
     * - roughness (z0)
     * - LAI
     * 
     * parameters depending on seasonality
    */
    ST_VegLib *cell;
    ST_VegLib *cell_u;
    cell = veglib + CLASS - 1;
    if (CLASS >=1 && CLASS <= 6)
    {
        // overstory
        cell_veg->LAI_o = cell->LAI[month - 1];
        cell_veg->z0_o = cell->Roughness[month - 1];
        cell_veg->Albedo_o = cell->Albedo[month - 1];
        cell_veg->d_o = cell->Displacement[month - 1];
        // understory
        cell_u = veglib + 10 - 1;
        cell_veg->LAI_u = cell_u->LAI[month - 1];
        cell_veg->z0_u = cell_u->Roughness[month - 1];
        cell_veg->Albedo_u = cell_u->Albedo[month - 1];
        cell_veg->d_u = cell_u->Displacement[month - 1];
    }
    else if (CLASS > 6 && CLASS <= 11)
    {
        // only understory
        cell_veg->LAI_u = cell->LAI[month - 1];
        cell_veg->z0_u = cell->Roughness[month - 1];
        cell_veg->Albedo_u = cell->Albedo[month - 1];
        cell_veg->d_u = cell->Displacement[month - 1];
    }
    else if (CLASS == 0)
    {
        // treated as bare soil/ground
        cell_veg->z0_u = 0.05;
        cell_veg->Albedo_u = 0.159;
        cell_veg->d_u = 0.2;
    }
    else
    {
        printf("Recognized Vegetation type. Program failled\n");
        exit(0);
    }
}

void main()
{
    char FP[] = "D:/xHM/example_data/VegeLib.txt";
    ST_VegLib veglib[11];
    Import_veglib(
        FP,
        veglib);
    printf("1-ID: %d\n", veglib[0].ID);
    printf("1-Rmin: %.3f\n", veglib[0].Rmin);

    printf("11-LAI: %.3f\n", *((veglib + 10)->LAI + 2));

    int ID = 5;
    int month = 1;
    // int Overstory;
    // double Rarc, Rmin, LAI, Roughness, Displacement, Albedo, SAI, CAN_TOP, CAN_BOTT, WIND_H;
    // LookUp_veglib(
    //     veglib, ID, month,
    //     &Overstory, &Rarc, &Rmin,
    //     &LAI, &Roughness, &Displacement, &Albedo,
    //     &SAI, &CAN_TOP, &CAN_BOTT, &WIND_H);
    int CLASS=0;
    ST_CELL_VEG cell_veg;
    Lookup_VegLib_CELL(veglib, CLASS, &cell_veg);
    
    Lookup_VegLib_CELL_MON(
        veglib,
        CLASS,
        month,
        &cell_veg);
    
    printf(
        "%10s%10s%10s%10s%10s%10s\n",
        "CLASS", "month", "Understory", 
        "CAN_TOP", "WIND_H", "Rpc"
    );
    printf(
        "%10d%10d%10d%10.3f%10.3f%10.3f\n",
        CLASS, month, cell_veg.Understory, 
        cell_veg.CAN_H, cell_veg.CAN_RZ, cell_veg.Rpc
    );

    printf(
        "Overstory:\n%10s%10s%10s%10s%10s\n",
        "LAI", "Albedo", "disp", "rough", "Rsmin"
    );
    printf(
        "%10.3f%10.3f%10.3f%10.3f%10.3f\n",
        cell_veg.LAI_o, cell_veg.Albedo_o, cell_veg.d_o, 
        cell_veg.z0_o, cell_veg.rs_min_o
    );

    printf(
        "Understory:\n%10s%10s%10s%10s%10s\n",
        "LAI", "Albedo", "disp", "rough", "Rsmin"
    );
    printf(
        "%10.3f%10.3f%10.3f%10.3f%10.3f\n",
        cell_veg.LAI_u, cell_veg.Albedo_u, cell_veg.d_u, 
        cell_veg.z0_u, cell_veg.rs_min_u
    );
}
