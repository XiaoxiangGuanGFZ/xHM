#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "xHM_def.h"


int main() {

    double Ta[] = {19.5, 20.5, 25.5, 30, 27.4, 20.5};
    double Tmin[] = {10.5, 10.5, 15.5, 15, 15.4, 10.5};
    double Tmax[] = {25.5, 26.5, 30.5, 38, 35.4, 29.5};
    double Wv[] = {1.52, 1.56, 2.23, 1.5, 5.0, 2.36};
    double RH[] = {50.0, 25.0, 56.0, 78.0, 45.0, 55.0};
    double Pa[] = {0.894, 0.589, 1.01, 0.958, 0.789, 0.855};
    double n[] = {9, 10, 8, 8.9, 7.5, 4.5};
    double J[] = {1, 2, 3, 4, 5, 6};
    double lat = 30.5;
    int i = 0;
    double ET;
    for (i = 0; i < 6; i++) {
        ET = penman(Ta[i], Tmax[i], Tmin[i], RH[i], Wv[i], Pa[i], n[i], J[i], lat );
        printf("The day %3.0f: %6.3f\n", J[i], ET);
    }

    return 0;
};