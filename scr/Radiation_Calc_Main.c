
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Radiation_Calc.h"


int main() {
    int year = 1999;
    int month = 1;
    int day[] = {1,2,3,4,5,6};
    double lat = 30.5;
    double Ta[] = {19.5, 20.5, 25.5, 30, 27.4, 20.5};
    double Tmin[] = {10.5, 10.5, 15.5, 15, 15.4, 10.5};
    double Tmax[] = {25.5, 26.5, 30.5, 38, 35.4, 29.5};
    double Wv[] = {1.52, 1.56, 2.23, 1.5, 5.0, 2.36};
    double RH[] = {50.0, 25.0, 56.0, 78.0, 45.0, 55.0};
    double Pa[] = {0.894, 0.589, 1.01, 0.958, 0.789, 0.855};
    double n[] = {9, 10, 8, 8.9, 7.5, 4.5};
    double J[] = {1, 2, 3, 4, 5, 6};
    int i = 0;
    double ET;
    double R_sky, R_short, R_long;
    double albedo_forest = 0.13;
    double LAI = 1.5;
    double albedo_snow = 0.5;
    double FF = 0.5;

    printf("Date \t R_short \t R_long \n");
    for (i = 0; i < 6; i++) {
        R_sky = Radiation_downward_short(
            year,
            month,
            day[i],
            lat,  // the latitute of the location
            n[i],   // sunshine duration in a day, hours
            0.25, 0.5    
        );
        R_short = Radiation_short_surface(
            R_sky, albedo_forest,  LAI, albedo_snow 
        );
        R_long = Radiation_long_surface(
            Ta[i], RH[i], FF
        );
        
        printf("%d-%d-%d \t", year, month, day[i]);
        printf(" %6.3f \t %6.3f \n", R_short, R_long);
    }

    return 0;
};

