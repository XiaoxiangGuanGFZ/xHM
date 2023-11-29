#ifndef SOIL_PERCOLATION
#define SOIL_PERCOLATION


double Percolation(
    double Soil_Moisture,
    double Percolation_in,
    double Soil_layer_thickness,
    double Soil_Porosity,
    double Soil_Residual,
    double Soil_Conduct_Sat,
    double Soil_PoreSize_index,
    int step_time 
);

double Soil_Hydro_Conductivity(
    double Soil_Moisture,
    double Soil_Porosity,
    double Soil_Residual,
    double Soil_Conduct_Sat,
    double Soil_PoreSize_index
);


#endif