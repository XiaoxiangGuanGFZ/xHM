/*
 * SUMMARY:      HM_ST.h
 * USAGE:        define data structures for the hydrological model level
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:   
 * DESCRIP-END.
 * 
 * COMMENTS:
 * 
 *
 */

#ifndef HM_ST
#define HM_ST

typedef struct 
{
    /* data */
    double SM_Upper;                /* soil moisture: upper soil layer, FRAC */
    double SM_Lower;                /* soil moisture: lower soil layer, FRAC */
    double SW_Infiltration;         /* water infiltration from ground surface, [m] */
    double SW_Percolation_Upper;    /* water percolation from upper soil layer, [m] */
    double SW_Percolation_Lower;    /* water percolation from lower soil layer, [m] */
    double SW_SR_Infil;             /* surface runoff from excess-infiltration, [m] */
    double SW_SR_Satur;             /* surface runoff from saturation, [m] */
    
} CELL_SOIL_VAR;


typedef struct 
{
    short Rs;
    short L_sky;
    short Rno;
    short Rnu;

    short Ep;
    short EI_o;
    short ET_o;
    short ET_u;
    short ET_u;
    short Es;

    short Interception_o;
    short Interception_u;
    short SM_Upper;
    short SM_Lower;
    short SW_Run_Infil;
    short SW_Run_Satur;
    short SW_Infiltration;
    short SW_Percolation_Upper;
    short SW_Percolation_Lower;

} OUT_NAME_LIST;


#endif


