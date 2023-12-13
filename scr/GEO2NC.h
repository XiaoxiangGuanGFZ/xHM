#ifndef GEO2NC
#define GEO2NC

int Import_data(
    char FP[],
    int **rdata,
    ST_Header *fp_header,
    int display
);

int Header_read(
    FILE *fp_geo,
    ST_Header *HD,
    int display
);

void Import_raster(
    int *rdata,
    FILE *fp_geo,
    int ncols,
    int nrows
);


void handle_error(int status);

#endif

