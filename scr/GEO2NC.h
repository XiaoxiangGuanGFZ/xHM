#ifndef GEO2NC
#define GEO2NC

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
    int cellsize_m;
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


#endif

