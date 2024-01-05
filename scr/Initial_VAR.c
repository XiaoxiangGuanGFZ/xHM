/*
 * SUMMARY:      Initial_VAR.c
 * USAGE:        intialize the variables structure
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Jan-2024
 * DESCRIPTION:  intialize the structures and assign the initial values 
 *               for the members
 * 
 * DESCRIP-END.
 * FUNCTIONS:    Initialize_RADIA();Initialize_ET();
 *               Initialize_SOIL();
 * 
 * COMMENTS:
 * see the details and member explanation in "HM_ST.h"
 * 
 * References:
 * 
 * 
*/

#include <stdlib.h>
#include "HM_ST.h"
#include "Initial_VAR.h"

void Initialize_RADIA(
    CELL_VAR_RADIA *st)
{
    for (size_t i = 0; i < sizeof(st) / sizeof(double); i++)
    {
        double *currentMember = ((double *)st) + i;
        *currentMember = 0.0;
    }
}

void Initialize_ET(
    CELL_VAR_ET *st)
{
    for (size_t i = 0; i < sizeof(st) / sizeof(double); i++)
    {
        double *currentMember = ((double *)st) + i;
        *currentMember = 0.0;
    }
}

void Initialize_SOIL(
    CELL_SOIL_VAR *st)
{
    for (size_t i = 0; i < sizeof(st) / sizeof(double); i++)
    {
        double *currentMember = ((double *)st) + i;
        *currentMember = 0.0;
    }
}
