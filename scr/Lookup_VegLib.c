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
 * REFERENCES:
 *
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
#include "Lookup_VegLib.h"

void Import_veglib(
    char FP[],
    ST_VegLib veglib[]
)
{
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
    int month = 5;
    int Overstory;
    double Rarc, Rmin, LAI, Roughness, Displacement, Albedo, SAI, CAN_TOP, CAN_BOTT, WIND_H;
    LookUp_veglib(
        veglib, ID, month,
        &Overstory, &Rarc, &Rmin,
        &LAI, &Roughness, &Displacement, &Albedo,
        &SAI, &CAN_TOP, &CAN_BOTT, &WIND_H);
}
