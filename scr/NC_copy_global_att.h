#ifndef NC_COPY_GLOBAL_ATT
#define NC_COPY_GLOBAL_ATT

#define ERR(e) {printf("Error: %s\n", nc_strerror(e)); return e;}

int copy_global_attributes(int input_ncid, int output_ncid);

void handle_error(int status);

#endif

