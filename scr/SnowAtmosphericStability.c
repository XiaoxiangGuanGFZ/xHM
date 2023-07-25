/*
    In the presence of a snow cover, aerodynamic resistance is typically 
    corrected for atmospheric stability according to the 
    bulk Richardson’s number (Rib). The latter is a dimensionless 
    ratio relating the buoyant and mechanical forces (i.e., turbulent eddies) 
    acting on a parcel of air [Anderson, 1976]
    */

double RichardsonNumber(
    double Tem_Air,  // air temperature, [celsius degree]
    double Tem_snow,  // temperature of snow surface, [celsius degree]
    double WindSpeed_z, // wind speed at the height of z, [m/s]
    double z         // the height (with unit of [m]) of the layer where the air temperature and wind speed are recored
){
    
    double Ri_b; // Richardson’s number
    Ri_b = 2 * g * z * (Tem_Air - Tem_snow) / (Tem_Air + Tem_snow) / pow(WindSpeed_z, 2);
    return Ri_b;
}


double AerodynamicResistance(
    double r_as, // the aerodynamic resistance between the snow surface and the near-surface reference height
    double Ri_b, // Richardson’s number
){
    double Ri_cr = 0.2; 
    double r_as_update; 
    /* Ri_cr is the critical value of the Richardson’s number (commonly taken as 0.2).*/
    if (Ri_b < 0) {
        /*unstable conditions*/
        r_as_update = r_as / pow(1 - 16 * Ri_b, 0.5);
    } else if (Ri_b <= Ri_cr)
    {
        /*stable conditions*/
        r_as_update = r_as / pow(1 - Ri_b / Ri_cr, 2);
    }
    
    return r_as_update;
}
