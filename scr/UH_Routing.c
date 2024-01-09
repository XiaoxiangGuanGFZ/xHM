/*
 * SUMMARY:      UH_Routing.c
 * USAGE:        Calculate overland runoff routing using Unit Hydrograph (UH) 
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  Calculate overland runoff using UH, over a gridded surface. 
 *               the UH is derived based on topography (DEM). 
 * DESCRIP-END.
 * FUNCTIONS:    Grid_Slope(), Grid_SlopeArea(), Grid_Velocity()
 *                  Grid_FlowTime(), Grid_UH(), Grid_Outlets(), 
 *                  Grid_OutletMask(), UH_Generation()
 * 
 * COMMENTS:
 * - Grid_Slope():          derive the slope from DEM and flow direction (D8)
 * - Grid_SlopeArea():      compute the slope-area term for each grid cell
 * - Grid_Velocity():       assign flow velocity to each grid cell
 * - Grid_FlowTime():       compute the flow time of each grid to the outlet
 * - Grid_UH():             generate UH for each grid cell
 * - Grid_Outlets():        extract the number and coordinates (row and col index) of outlets
 * - Grid_OutletMask():     extract the mask (the upstream region) of an outlet (based on coordinates)
 * - UH_Generation():       generate the Unit Hydrograph for multiple outlets
 * REFERENCES:
 * 
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "GEO_ST.h"
#include <UH_Generation.h>
#include <UH_Routing.h>

void UH_Import(
    int ncID_UH
)
{

}
