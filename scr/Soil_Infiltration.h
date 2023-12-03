#ifndef SOIL_INFIL
#define SOIL_INFIL

double Soil_Infiltration(
    double Water_input,
    double Soil_Moisture,
    double Soil_Porosity,
    double Soil_Conduct_Sat,
    double Air_Entry_Pres,
    double b, 
    int step_time
);

#endif
