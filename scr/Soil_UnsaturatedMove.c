/*
 * SUMMARY:      Soil_UnsaturatedMove.c
 * USAGE:        iterate the soil water movement
 *               in unsaturated zone 
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Jan-2024
 * DESCRIPTION:  the water movement in unsaturated soil zone here considers
 *               infiltration and percolation.
 *  
 * DESCRIP-END.
 * FUNCTIONS:    UnsaturatedWaterMove(), 
 * 
 * COMMENTS:
 * The desorption is calculated during computing the 
 * actual soil evaporation. 
 * REFERENCES:
 * 
 *
 */

/**************************************************************************
 * VARIABLEs:
    double Water_input                - water input rate on the ground surface, [m/h]
    double ET_o                       - overstory transpiration, [m]
    double ET_u                       - understory transpiration, [m]
    double Es                         - soil evaporation, [m]
    double *Soil_Moisture_upper       - the soil moisture in the upper soil layer, [fraction]
    double *Soil_Moisture_lower       - the soil moisture in the lower soil layer, [fraction]
    double *SW_Infiltration           - infiltration from ground surface into upper soil layer, [m]
    double *SW_Percolation_Upper      - percolated water from upper soil layer, [m]
    double *SW_Percolation_Lower      - percolated water from lower soil layer, [m]
    double SWV_gw                     - the volume of water supplied by a rising water table, [m]
    double SWV_rf                     - the volume of return flow (generated when a rising water table reaches the ground surface), [m]
    double *SW_Run_Infil              - generated surface runoff from excess-infiltration, [m]
    double *SW_Run_Satur              - generated surface runoff from soil saturation, [m]
    double Soil_thickness_upper       - the thickness of upper soil layer, [m]
    double Soil_thickness_lower       - the thickness of lower soil layer, [m]
    ST_SOIL_LIB_CELL *cell_soil_lib   - soil parameters library for a cell, see "Lookup_SoilLib.h"
    int STEP_TIME                     - time step of model simulation

******************************/

#include <stdio.h>
#include "Constants.h"
#include "Soil_UnsaturatedMove.h"
#include "Lookup_SoilLib.h"


void UnsaturatedWaterMove(
    double Water_input,
    double ET_o,
    double ET_u,
    double Es,
    double *Soil_Moisture_upper,
    double *Soil_Moisture_lower,
    double *SW_Infiltration,
    double *SW_Percolation_Upper,
    double *SW_Percolation_Lower,
    double SWV_gw,
    double SWV_rf,
    double *SW_Run_Infil,
    double *SW_Run_Satur,
    double Soil_thickness_upper,
    double Soil_thickness_lower,
    ST_SOIL_LIB_CELL *cell_soil_lib,
    int STEP_TIME
)
{
    /**************************************
     * iterate the unsaturated zone soil water movement:
     * infiltration and percolation;
     * The excess-infiltation and saturation runoff generation are considered
    */
    if (Water_input > 0.0)
    {
        *SW_Infiltration = Soil_Infiltration(
            Water_input, *Soil_Moisture_upper,
            cell_soil_lib->Topsoil->Porosity / 100,
            cell_soil_lib->Topsoil->SatHydrauCond,
            cell_soil_lib->Topsoil->AirEntryPresHead,
            cell_soil_lib->Topsoil->PoreSizeDisP,
            STEP_TIME);
        if (Water_input * STEP_TIME > *SW_Infiltration)
        {
            *SW_Run_Infil = Water_input * STEP_TIME - *SW_Infiltration;
        }
        else
        {
            *SW_Run_Infil = 0.0;
        }
    }
    else
    {
        *SW_Infiltration = 0.0;
        *SW_Run_Infil = 0.0;
    }

    *SW_Percolation_Upper = Percolation(
        *Soil_Moisture_upper,
        *SW_Infiltration,
        Soil_thickness_upper,
        cell_soil_lib->Topsoil->Porosity / 100,
        cell_soil_lib->Topsoil->Residual / 100,
        cell_soil_lib->Topsoil->SatHydrauCond,
        1 / cell_soil_lib->Topsoil->PoreSizeDisP,
        STEP_TIME);
    
    *SW_Percolation_Lower = Percolation(
        *Soil_Moisture_lower,
        *SW_Percolation_Upper,
        Soil_thickness_lower,
        cell_soil_lib->Subsoil->Porosity / 100,
        cell_soil_lib->Subsoil->Residual / 100,
        cell_soil_lib->Subsoil->SatHydrauCond,
        1 / cell_soil_lib->Subsoil->PoreSizeDisP,
        STEP_TIME);

    double SM_buff;
    *SW_Run_Satur = 0.0;
    /********
     * upper soil layer
    */
    SM_buff = (SW_Infiltration - SW_Percolation_Upper - ET_o - ET_u - Es + SWV_gw - SWV_rf) /
                  Soil_thickness_upper +
              *Soil_Moisture_upper;
    if (SM_buff > cell_soil_lib->Topsoil->Porosity / 100)
    {
        *SW_Run_Satur += (SM_buff - cell_soil_lib->Topsoil->Porosity / 100) * Soil_thickness_upper;
        *Soil_Moisture_upper = cell_soil_lib->Topsoil->Porosity / 100;
    }
    else if (SM_buff < 0.0)
    {
        *SW_Run_Satur += 0.0;
        *Soil_Moisture_upper = 0.0;
    }
    else
    {
        *SW_Run_Satur += 0.0;
        *Soil_Moisture_upper = SM_buff;
    }

    /********
     * lower soil layer
    */
    SM_buff = (*SW_Percolation_Upper - *SW_Percolation_Lower - ET_o + SWV_gw) /
                  Soil_thickness_lower +
              *Soil_Moisture_lower;
    if (SM_buff > cell_soil_lib->Subsoil->Porosity / 100)
    {
        *SW_Run_Satur += (SM_buff - cell_soil_lib->Subsoil->Porosity / 100) * Soil_thickness_upper;
        *Soil_Moisture_lower = cell_soil_lib->Subsoil->Porosity / 100;
    }
    else if (SM_buff < 0.0)
    {
        *SW_Run_Satur += 0.0;
        *Soil_Moisture_lower = 0.0;
    }
    else
    {
        *SW_Run_Satur += 0.0;
        *Soil_Moisture_lower = SM_buff;
    }

}