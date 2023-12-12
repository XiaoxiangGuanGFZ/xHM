#include <stdio.h>
#include <stdlib.h>
#include <netcdf.h>
#include <string.h>
#include <math.h>

#define MAXCHAR 10000

typedef struct 
{
    char FP_DEM[MAXCHAR];
    char FP_FDR[MAXCHAR];
    char FP_FAC[MAXCHAR];
    char FP_STR[MAXCHAR];
    char FP_OUTLET[MAXCHAR];
    char FP_GEONC[MAXCHAR];
    char Projection[MAXCHAR];
    char Datum[MAXCHAR];
    char Spheroid[MAXCHAR];
    char Units[MAXCHAR];
    char Zunits[MAXCHAR];
} ST_Para;

typedef struct 
{
    int ncols;
    int nrows;
    double xllcorner;
    double yllcorner;
    double cellsize;
    int NODATA_value;

} ST_Header;


void handle_error(int status);
void print2D(
    int *ptr,
    int m,
    int n
);
int Header_read(
    FILE *fp_geo,
    ST_Header *HD,
    int display
);
void Import_para(
    char FP[],
    ST_Para *gp,
    int display    
);
void Para_check(
    ST_Para *gp
);
void Import_raster(
    int *rdata,
    FILE *fp_geo,
    int ncols,
    int nrows
);

int Import_data(
    char FP[],
    int **rdata,
    ST_Header *fp_header,
    int display
);

int main(int argc, char * argv[])
{
    char IO[2] = "-I";
    strcpy(IO, *(++argv));
    if (strcmp(IO, "-I") == 0) 
    {
        printf("---- Transfer 2D array data from ASCII to NetCDF format! ----\n");
    } 
    else
    {
        printf("---- Transfer 2D array data from NetCDF to ASCII format! ----\n");
    }

    ST_Para GP;
    Import_para(
        *(++argv),
        &GP,
        1);
    Para_check(&GP);

    if (strcmp(IO, "-I") == 0)
    {
        // DEM
        ST_Header HD_dem;
        int *rdata_dem;
        Import_data(GP.FP_DEM, &rdata_dem, &HD_dem, 1); 
        /**
         * transfer the pointer to a pointer (address of an address)
         * in order to munipulate the rdata_dem 
         *  (bring the results/cahnges) from function execution
        */

        // flow direction: fdr
        ST_Header HD_fdr;
        int *rdata_fdr;
        Import_data(GP.FP_FDR, &rdata_fdr, &HD_fdr, 0);

        // flow accumulation: fac
        ST_Header HD_fac;
        int *rdata_fac;
        Import_data(GP.FP_FAC, &rdata_fac, &HD_fac, 0);

        // stream: str
        ST_Header HD_str;
        int *rdata_str;
        Import_data(GP.FP_STR, &rdata_str, &HD_str, 0);
        
        // outlet
        ST_Header HD_outlet;
        int *rdata_outlet;
        Import_data(GP.FP_OUTLET, &rdata_outlet, &HD_outlet, 0);
        
        /************* create nc file ****************/
        int i, j;
        double *data_lon;
        double *data_lat;
        data_lon = (double *)malloc(sizeof(double) * HD_dem.ncols);
        data_lat = (double *)malloc(sizeof(double) * HD_dem.nrows);

        for (j = 0; j < HD_dem.ncols; j++)
        {
            *(data_lon + j) = HD_dem.xllcorner + HD_dem.cellsize * j;
        }
        for (i = 0; i < HD_dem.nrows; i++)
        {
            *(data_lat + i) = HD_dem.yllcorner + HD_dem.cellsize * (HD_dem.nrows - i - 1);
        }
        

        int IO_status;
        int ncID;
        int dimID_lon, dimID_lat;
        int varID_lon, varID_lat;
        int varID_dem, varID_fdr, varID_fac, varID_str, varID_outlet;
        int old_fill_mode;

        IO_status = nc_create(GP.FP_GEONC, NC_CLOBBER, &ncID);
        if (IO_status != NC_NOERR)
            handle_error(IO_status);
        /**** define mode ****/ 
        nc_set_fill(ncID, NC_FILL, &old_fill_mode);
        nc_def_dim(ncID, "lon", HD_dem.ncols, &dimID_lon);
        nc_def_dim(ncID, "lat", HD_dem.nrows, &dimID_lat);

        int dims[2];
        int fdr_ranges[8] = {1,2,4,8,16,32,64,128};
        dims[0] = dimID_lat;
        dims[1] = dimID_lon;
        nc_def_var(ncID, "lon", NC_DOUBLE, 1, &dimID_lon, &varID_lon);
        nc_def_var(ncID, "lat", NC_DOUBLE, 1, &dimID_lat, &varID_lat);
        nc_def_var(ncID, "DEM", NC_INT, 2, dims, &varID_dem);
        nc_def_var(ncID, "FDR", NC_INT, 2, dims, &varID_fdr);
        nc_def_var(ncID, "FAC", NC_INT, 2, dims, &varID_fac);
        nc_def_var(ncID, "STR", NC_INT, 2, dims, &varID_str);
        nc_def_var(ncID, "OUTLET", NC_INT, 2, dims, &varID_outlet);

        /**** put attributes ****/
        nc_put_att_int(ncID, NC_GLOBAL, "ncols", NC_INT, 1, &HD_dem.ncols);
        nc_put_att_int(ncID, NC_GLOBAL, "nrows", NC_INT, 1, &HD_dem.nrows);
        nc_put_att_double(ncID, NC_GLOBAL, "xllcorner", NC_DOUBLE, 1, &HD_dem.xllcorner);
        nc_put_att_double(ncID, NC_GLOBAL, "yllcorner", NC_DOUBLE, 1, &HD_dem.yllcorner);
        nc_put_att_double(ncID, NC_GLOBAL, "cellsize", NC_DOUBLE, 1, &HD_dem.cellsize);

        nc_put_att_text(ncID, NC_GLOBAL, "Projection", 30, GP.Projection);
        nc_put_att_text(ncID, NC_GLOBAL, "Spheroid", 20L, GP.Spheroid);
        nc_put_att_text(ncID, NC_GLOBAL, "Units", 20L, GP.Units);
        nc_put_att_text(ncID, NC_GLOBAL, "Zunits", 20L, GP.Zunits);
        nc_put_att_text(ncID, NC_GLOBAL, "Datum", 20L, GP.Datum);
        
        nc_put_att_text(ncID, varID_dem, "long_name", 40L, "Digital Elevation Model");
        nc_put_att_text(ncID, varID_dem, "units", 40L, "meters");
        nc_put_att_int(ncID, varID_dem, "missing_value", NC_INT, 1, &HD_dem.NODATA_value);

        nc_put_att_text(ncID, varID_fdr, "long_name", 40L, "flow direction");
        nc_put_att_text(ncID, varID_fdr, "units", 40L, "NULL");
        nc_put_att_int(ncID, varID_fdr, "missing_value", NC_INT, 1, &HD_dem.NODATA_value);
        nc_put_att_int(ncID, varID_fdr, "valid_range", NC_INT, 8, fdr_ranges);
        nc_put_att_text(ncID, varID_fdr, "description", 200L, \
            "D8 direction type: 1(East/Right); 2(SouthEast/LowerRight); 4(South/Down); 8(SouthWest/Lowerleft); 16(West/Left); 32(NorthWest/UpperLeft); 64(North/Up); 128(NorthEst/UpperRight)");
        
        nc_put_att_text(ncID, varID_fac, "long_name", 40L, "flow accumulation");
        nc_put_att_text(ncID, varID_fac, "units", 40L, "number of cells");
        nc_put_att_int(ncID, varID_fac, "missing_value", NC_INT, 1, &HD_dem.NODATA_value);

        nc_put_att_text(ncID, varID_str, "long_name", 40L, "stream");
        nc_put_att_int(ncID, varID_str, "missing_value", NC_INT, 1, &HD_dem.NODATA_value);
        nc_put_att_int(ncID, varID_str, "valid_range", NC_INT, 1, (int[]){1});
        nc_put_att_text(ncID, varID_str, "description", 40L, "1: this cell is the stream");

        nc_put_att_text(ncID, varID_outlet, "long_name", 40L, "outlet of watersheds");
        nc_put_att_int(ncID, varID_outlet, "missing_value", NC_INT, 1, &HD_dem.NODATA_value);
        nc_put_att_int(ncID, varID_outlet, "valid_range", NC_INT, 1, (int[]){1});
        nc_put_att_text(ncID, varID_outlet, "description", 40L, "1: this cell is an outlet");


        nc_enddef(ncID);
        /*********  data mode *******/
        size_t start[2] = {0, 0};
        size_t counts[2];
        counts[0] = HD_dem.nrows;
        counts[1] = HD_dem.ncols;
        nc_put_var_double(ncID, varID_lon, data_lon);
        nc_put_var_double(ncID, varID_lat, data_lat);
        
        nc_put_vara_int(ncID, varID_dem, start, counts, rdata_dem);
        nc_put_vara_int(ncID, varID_fdr, start, counts, rdata_fdr);
        nc_put_vara_int(ncID, varID_fac, start, counts, rdata_fac);
        nc_put_vara_int(ncID, varID_str, start, counts, rdata_str);
        nc_put_vara_int(ncID, varID_outlet, start, counts, rdata_outlet);

        nc_close(ncID);
    }
    
    return 1;
}

void Import_para(
    char FP[],
    ST_Para *gp,
    int display    
)
{
    char row[MAXCHAR];
    char S1[MAXCHAR];
    char S2[MAXCHAR];

    FILE *fp;
    if ((fp=fopen(FP, "r")) == NULL) {
        printf("cannot open global parameter file\n");
        exit(0);
    }

    while (fgets(row, MAXCHAR, fp) != NULL)
    {
        if (sscanf(row, "%[^,],%s", S1, S2) == 2)
        {
            if (display == 1)
            {
                printf("%s,%s\n", S1, S2);
            }
            if (strcmp(S1, "FP_DEM") == 0)
            {
                strcpy(gp->FP_DEM, S2);
            }
            else if (strcmp(S1, "FP_FDR") == 0)
            {
                strcpy(gp->FP_FDR, S2);
            }
            else if (strcmp(S1, "FP_FAC") == 0)
            {
                strcpy(gp->FP_FAC, S2);
            }
            else if (strcmp(S1, "FP_STR") == 0)
            {
                strcpy(gp->FP_STR, S2);
            }
            else if (strcmp(S1, "FP_OUTLET") == 0)
            {
                strcpy(gp->FP_OUTLET, S2);
            }
            else if (strcmp(S1, "FP_GEONC") == 0)
            {
                strcpy(gp->FP_GEONC, S2);
            }
            else if (strcmp(S1, "Projection") == 0)
            {
                strcpy(gp->Projection, S2);
            }
            else if (strcmp(S1, "Datum") == 0)
            {
                strcpy(gp->Datum, S2);
            }
            else if (strcmp(S1, "Spheroid") == 0)
            {
                strcpy(gp->Spheroid, S2);
            }
            else if (strcmp(S1, "Units") == 0)
            {
                strcpy(gp->Units, S2);
            }
            else if (strcmp(S1, "Zunits") == 0)
            {
                strcpy(gp->Zunits, S2);
            }
            else
            {
                printf("Unrecognized field!");
                exit(0);
            }
        }
    }
}

void Para_check(
    ST_Para *gp
)
{
    if (gp->FP_DEM[0] == '\0'){
        printf("FP_DEM field is empty!\n");
        exit(0);
    }
    if (gp->FP_FAC[0] == '\0'){
        printf("FP_FAC field is empty!\n");
        exit(0);
    }
    if (gp->FP_FDR[0] == '\0'){
        printf("FP_FDR field is empty!\n");
        exit(0);
    }
    if (gp->FP_STR[0] == '\0'){
        printf("FP_STR field is empty!\n");
        exit(0);
    }
    if (gp->FP_OUTLET[0] == '\0'){
        printf("FP_OUTLET field is empty!\n");
        exit(0);
    }
    if (gp->FP_GEONC[0] == '\0'){
        printf("FP_GEONC field is empty!\n");
        exit(0);
    }
    if (gp->Spheroid[0] == '\0'){
        printf("Spheroid field is empty!\n");
        exit(0);
    }
    if (gp->Units[0] == '\0'){
        printf("Units field is empty!\n");
        exit(0);
    }
    if (gp->Zunits[0] == '\0'){
        printf("Zunits field is empty!\n");
        exit(0);
    }
    if (gp->Projection[0] == '\0'){
        printf("Projection field is empty!\n");
        exit(0);
    }
    if (gp->Datum[0] == '\0'){
        printf("Datum field is empty!\n");
        exit(0);
    }
}


void handle_error(int status)
{
    if (status != NC_NOERR)
    {
        fprintf(stderr, "%s\n", nc_strerror(status));
        exit(-1);
    }
}

void print2D(
    int *ptr,
    int m,
    int n
)
{
    int i,j;
    for (i = 0; i < m; i++)
    {
        for (j = 0; j < n; j++)
        {
            printf(
                "%.2f\t", *((ptr + i * n)+j)
            );
        }
        printf("\n");
    }    
}

int Header_read(
    FILE *fp_geo,
    ST_Header *HD,
    int display)
{
    char row[MAXCHAR];
    char s1[MAXCHAR];

    if (fgets(row, MAXCHAR, fp_geo) != NULL)
    {
        sscanf(row, "%s %d", s1, &HD->ncols);
        if (display == 1)
        {
            printf("%s: %d\n", s1, HD->ncols);
        }
        if (fgets(row, MAXCHAR, fp_geo) != NULL)
        {
            sscanf(row, "%s %d", s1, &HD->nrows);
            if (display == 1)
            {
                printf("%s: %d\n", s1, HD->nrows);
            }
            if (fgets(row, MAXCHAR, fp_geo) != NULL)
            {
                sscanf(row, "%s %lf", s1, &HD->xllcorner);
                if (display == 1)
                {
                    printf("%s: %.12lf\n", s1, HD->xllcorner);
                }
                if (fgets(row, MAXCHAR, fp_geo) != NULL)
                {
                    sscanf(row, "%s %lf", s1, &HD->yllcorner);
                    if (display == 1)
                    {
                        printf("%s: %.12lf\n", s1, HD->yllcorner);
                    }
                    if (fgets(row, MAXCHAR, fp_geo) != NULL)
                    {
                        sscanf(row, "%s %lf", s1, &HD->cellsize);
                        if (display == 1)
                        {
                            printf("%s: %.12lf\n", s1, HD->cellsize);
                        }
                        if (fgets(row, MAXCHAR, fp_geo) != NULL)
                        {
                            sscanf(row, "%s %d", s1, &HD->NODATA_value);
                            if (display == 1)
                            {
                                printf("%s: %d\n", s1, HD->NODATA_value);
                            }
                        }
                        else
                        {
                            printf("reading the 5th row in DEM data file failed!\n");
                            exit(0);
                        }
                    }
                    else
                    {
                        printf("reading the 5th row in DEM data file failed!\n");
                        exit(0);
                    }
                }
                else
                {
                    printf("reading the 4th row in DEM data file failed!\n");
                    exit(0);
                }
            }
            else
            {
                printf("reading the 3rd row in DEM data file failed!\n");
                exit(0);
            }
        }
        else
        {
            printf("reading the 2nd row in DEM data file failed!\n");
            exit(0);
        }
    }
    else
    {
        printf("reading the first row in DEM data file failed!\n");
        exit(0);
    }
    return 1;
}

void Import_raster(
    int *rdata,
    FILE *fp_geo,
    int ncols,
    int nrows
)
{
    
    if (rdata == NULL)
    {
        printf("Error allocating memory");
        exit(-1);
    }
    int i, j;
    j = 0;
    char row[MAXCHAR];
    for (j = 0; j < nrows; j++)
    {
        for (i = 0; i < ncols; i++)
        {
            if (fscanf(fp_geo, "%d", rdata + j * ncols + i) != 1)
            {
                fprintf(stderr, "Error reading integer in %d th row from file.\n", j);
                exit(-2);
            }
        }
    }
}

int Import_data(
    char FP[],
    int **rdata,
    ST_Header *fp_header,
    int display
)
{
    FILE *fp;
    int IO_status;
    if ((fp = fopen(FP, "r")) == NULL)
    {
        printf("failed to open %s!\n", FP);
        exit(0);
    }
    IO_status = Header_read(fp, fp_header, display);
    if (IO_status == 1)
    {
        *rdata = (int *)malloc(sizeof(int) * fp_header->ncols * fp_header->nrows);
        Import_raster(*rdata, fp, fp_header->ncols, fp_header->nrows);
    }
    fclose(fp);
    return 1;
}
