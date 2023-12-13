#ifndef GEO_ST
#define GEO_ST

#define MAXCHAR 10000

typedef struct 
{
    char FP_DEM[MAXCHAR];     /* file path and name of DEM data */
    char FP_FDR[MAXCHAR];     /* file path and name of flow direction (fdr) data*/
    char FP_FAC[MAXCHAR];     /* file path and name of flow accumulation (fac) data */
    char FP_STR[MAXCHAR];     /* file path and name of stream (str) data */
    char FP_OUTLET[MAXCHAR];  /* file path and name of watershed outlet data */
    char FP_GEONC[MAXCHAR];   /* file path and name of NetCDF data*/
    char Projection[MAXCHAR]; /* projection of the raster data */
    char Datum[MAXCHAR];      
    char Spheroid[MAXCHAR]; 
    char Units[MAXCHAR];      /* unit of the coordinate system. DD: decimal */
    char Zunits[MAXCHAR];
    int cellsize_m;           /* cell size in the unit of meters; spatial resolution */
} ST_GEO2NC_Para;

typedef struct 
{
    int ncols;         /* number of cell columns */
    int nrows;         /* number of cell rows */
    double xllcorner;  /* X-coordinate of the origin (by center or lower left corner of the cell) */
    double yllcorner;  /* Y-coordinate of the origin (by center or lower left corner of the cell) */
    double cellsize;   /* Cell size */
    int NODATA_value;  /* The input values to be NoData in the output raster */

} ST_Header;


#endif