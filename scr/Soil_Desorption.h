#ifndef SOIL_DESORPTION
#define SOIL_DESORPTION

double Soil_Desorption(
    double Soil_Moisture,
    double Soil_Conduct_Sat,
    double Soil_PoreSize_index,
    double Soil_Porosity,
    double Soil_Bubbling_pres,
    int step_time
);


#endif