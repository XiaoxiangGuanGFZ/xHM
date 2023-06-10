
double penman(
    double Ta, /*scalar: daily average air tempeature (celsius degree)*/
    double Tmax, /*scalar: daily maximum air temperature (celsius degree)*/
    double Tmin, /*scalar: daily minimum air temperature (celsius degree)*/
    double RH,   /*scalar: relative humidity (%)*/
    double Wv,   /*scalar: wind speed (m/s)*/
    double Pa,   /*scalar: atmospheric pressure (kPa)*/
    double n,    /*scalar: daylight duration (hours)*/
    double J,    /*scalar: the position of day in a year*/
    double lat   /*scalar: latitude (decimal degree)*/
){
    /*************
     * FAO56 Penman-Monteith model (Allen, 1980), mm/d
     * 
     * ***************/    
    // J: the number of day in a year, 1 for 1th January and 365 (or 366) for 31th December.
    // lat: latitude of the weather station: decimal °
    // n: sunshine duration, unit: hours
    // Wv: the wind speed at the height of 10m.
    
    /*here define the intermediate variables estimated during the calculation*/
    double u2, gamma, es, ea, delta, dr, del, w_s, Ra, N, Rs0, Rs, Rns, Rnl, Rn;
    double ET;  // the potential evapotranspiration (mm/d)

    u2 = U_2(Wv, 10.0);  //Wv*4.87/(log(67.8*10-5.42));  //u2: wind speed at the height of 2m；

    /*
    gamma: psychrometric constant (kPa⋅℃^(-1))
    干湿常数
    */
    gamma = 0.665 * pow(10, -3) * Pa ; 
    /* es and ea indicate saturation vapor pressure (kPa) and actual vapor pressure (kPa) */
    es = 0.5*(e0(Tmax) + e0(Tmin)); //es为饱和水汽压
    ea = RH * es / 100 ; //ea为实际水汽压，kPa

    /*
    * delta: the slope of saturation vapor pressure curve (kPa⋅℃ -1)/ 
      饱和水汽压曲线斜率（kPa*℃^-1）
    */
    delta = 4098 * e0(Ta) / pow(0.5*(Tmax+Tmin) + 237.3, 2.0);  
    
    //****计算地球外辐射，MJ/m2/d********
    dr = 1 + 0.033 * cos(2 * PI / 365 * J);  //the inverse relative distance between earth and sun.
    del = 0.408 * sin(2 * PI / 365 * J - 1.39) ; //the solar decimation (rad)
    w_s = acos( - tan(lat * PI / 180) * tan(del));    // the sunset hour angle (rad)
    /*Ra: extraterrestrial radiation (MJ⋅m-2 d-1)*/
    Ra = 37.59 * dr * (w_s * sin(lat * PI / 180) * sin(del) + cos(lat * PI / 180) * cos(del) * sin(w_s)); //Ra：地球外辐射
    // 37.59: 24(60)/PI*Gsc; Gsc is the solar constant and equals to 0.082MJ⋅m(-2)⋅min(-1)

    N = 24 / PI * w_s;  //the maximum possible duration of sunshine (h).
    Rs0 = (0.25 + 0.5) * Ra;  //clear-sky radiation,
    Rs = (0.25 + 0.5 * n/N) * Ra; //received solar radiation, (MJ⋅m-2 d-1).
    
    /*Rns: incoming net solar or shortwave radiation*/
    Rns = (1 - ALBEDO) * Rs; // ALBEDO: the albedo (or canopy reflection coefficient) and equals to 0.23
    
    /*Rnl: outgoing net longwave radiation*/
    // 4.903 * pow(10, -9): Stefan-Boltzmann constant
    Rnl = 4.903 * pow(10, -9) * (pow(Kelvin_tem(Tmax), 4.0) + pow(Kelvin_tem(Tmin), 4.0)) * 0.5 * (0.34 - 0.14*sqrt(ea)) * (1.35 * Rs/Rs0 - 0.35);
    Rn = Rns - Rnl;   // Rn: net radiation，MJ/m2/d
    
    /*es-ea: saturation vapour pressure deficit (kPa)*/
    ET = (0.408 * delta * Rn + gamma * 900 / (Ta + 273) * u2 * (es-ea)) / (delta + gamma * (1 + 0.34*u2));
    return ET;
}

double e0(
    double Ta /*scalar: air temperature (celsius degree)*/
) {
    /*
    estimate saturated water-vapor pressures (kPa) from air temperature
    */
    // 根据气温，计算得到瞬时饱和水汽压kPa
    return 0.6108 * exp(17.277 * Ta / (Ta + 273.3));
}

double U_2(
    double Wv,
    double height
){
    /*
    estimate the wind speed at the height of 2m from that of another height
    */
    return Wv*4.87/(log(67.8*height-5.42));
}

double Kelvin_tem(
    double tem
) {
    /*concert the temperature from celsius degree to Kelvin K */
    return tem + 273.15;
}
