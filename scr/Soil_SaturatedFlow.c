
/*
 * SUMMARY:      Soil_SaturatedFlow.c
 * USAGE:        Calculate the water movement in saturated soil zone
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Nov-2023
 * DESCRIPTION:  Calculate saturated water movement
 * DESCRIP-END.
 * FUNCTIONS:    
 *               ()
 * COMMENTS:
 * 
 * 
 */


/****************************************************************************
 * VARIABLEs:
 * double Soil_Thickness         - soil thickness, [m]
 * double Soil_Porosity          - soil porosity
 * double Soil_Conduct_Sat       - soil vertical saturated hydraulic conductivity, [m/h]
 * double Cell_WT_z              - depth (water table) into the soil profile (positive downward), [m] 
 * double *Cell_WT               - pointer to an array of water table depth in 8 adjacent cells, [m]
 * double step_space             - grid cell size, [m]
 * double *Cell_q                - pointer to an array of outflow from the cell to 8 adjacent cells,
 * double *Qout                  - total outflow from the cell
 * double b                      - pore size distribution parameter in Brooks and Corey 1964 formula
 * double n                      - the local power law exponent
 * double *F                     - pointer to an array of outflow fractions to 8 directions
 * 
 * 
 * REFERENCEs:
 * 
 * 
 * 
******************************************************************************/


#include <math.h>
#include <Soil_SaturatedFlow.h>


double Soil_Satu_grad
(
    /*******
     *  \gamma_{i,j,k}
     * 
    */
    double Soil_Conduct_Sat,
    double Soil_Thickness,
    double n,
    double Cell_WT_z,
    double Cell_WT_k,
    double step_space 
)
{
    double slope;
    if (
        Cell_WT_k <= Cell_WT_z
    ){
        slope = 0.0; 
    } else {
        slope = (Cell_WT_z - Cell_WT_k) / step_space;  // slope < 0.0
    }
    double gamma;
    gamma = - slope * (step_space * Soil_Conduct_Sat * Soil_Thickness / n);
    return gamma;
}

void Soil_Satu_Outflow(
    double Cell_WT_z,
    double *Cell_WT,
    double *Cell_q,
    double *F,
    double *Qout,
    double Soil_Conduct_Sat,
    double Soil_Thickness,
    double b,
    double step_space
)
{
    double n;
    n = 2 * b +3;
    double slope[8];
    double gamma[8];
    double gamma_sum = 0.0;
    double q[8];
    *Qout = 0.0;
    double h;
    h = pow(1 - Cell_WT_z/Soil_Thickness, n);
    int k = 0;
    for (k = 0; k < 8; k++)
    {
        *(gamma + k) = Soil_Satu_grad(
            Soil_Conduct_Sat,
            Soil_Thickness, 
            n,
            Cell_WT_z,
            *(Cell_WT + k),
            step_space
        );
        gamma_sum += *(gamma + k);
        *Qout += h * *(gamma + k);
    }
    if (gamma_sum <= 0.0)
    {
        for (k = 0; k < 8; k++)
        {
            *(q + k) = 0.0;
            *(F + k) = 0.0;
        }
    } else {
        for (k = 0; k < 8; k++)
        {
            *(F + k) = *(gamma + k) / gamma_sum;
            *(q + k) = *(F + k) * *Qout;
        }
    }
}


