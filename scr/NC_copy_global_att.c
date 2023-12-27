
/*
 * SUMMARY:      NC_copy_global_att.c
 * USAGE:        copy all global attributes from one nc file to another one
 * AUTHOR:       Xiaoxiang Guan
 * ORG:          Section Hydrology, GFZ
 * E-MAIL:       guan@gfz-potsdam.de
 * ORIG-DATE:    Dec-2023
 * DESCRIPTION:  copy global attributes and handle_error 
 * DESCRIP-END.
 * FUNCTIONS:    copy_global_attributes(), handle_error(), 
 * 
 * COMMENTS:
 * 
 *
 * REFERENCES:
 *
 */

/*****************************************************************
 * VARIABLEs:
 * int input_ncid               - netcdf file ID (attributes donor)
 * int output_ncid              - netcdf file ID (attributes receiver)
 * 
*/

#include <netcdf.h>

#include "NC_copy_global_att.h"

int copy_global_attributes(
    int input_ncid, 
    int output_ncid
) {
    int status, num_atts, i;
    char attname[NC_MAX_NAME + 1];
    nc_type xtype;
    size_t attlen;
    void *attvalue;

    // Get the number of attributes for NC_GLOBAL
    if ((status = nc_inq_natts(input_ncid, &num_atts)) != NC_NOERR) ERR(status);

    // Loop through each attribute
    for (i = 0; i < num_atts; i++) {
        // Get attribute information
        if ((status = nc_inq_attname(input_ncid, NC_GLOBAL, i, attname)) != NC_NOERR) ERR(status);
        if ((status = nc_inq_att(input_ncid, NC_GLOBAL, attname, &xtype, &attlen)) != NC_NOERR) ERR(status);

        // Allocate memory for attribute value
        attvalue = malloc(attlen);
        if (attvalue == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            return NC_ENOMEM;
        }

        // Read attribute value
        if ((status = nc_get_att(input_ncid, NC_GLOBAL, attname, attvalue)) != NC_NOERR) {
            free(attvalue);
            ERR(status);
        }

        // Write attribute to the output file
        if ((status = nc_put_att(output_ncid, NC_GLOBAL, attname, xtype, attlen, attvalue)) != NC_NOERR) {
            free(attvalue);
            ERR(status);
        }

        // Free allocated memory
        free(attvalue);
    }

    return NC_NOERR;
}

void handle_error(int status)
{
    /*****************************
     * error handling function for 
     * netcdf data reading and writing
    */
    if (status != NC_NOERR)
    {
        fprintf(stderr, "%s\n", nc_strerror(status));
        exit(-1);
    }
}

