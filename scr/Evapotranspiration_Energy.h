#ifndef ET_ENERGY
#define ET_ENERGY


void Radiation_net(
    double Rs,
    double L_sky,
    double *Rno,
    double *Rnu,
    double Frac_canopy,
    double Ref_o,
    double Ref_u,
    double Tem_o,
    double Tem_u,
    double Tem_s,
    double LAIo,
    int Vtype
);

double Emissivity(
    double Tem   /* temperature of the unity, [celsuis degree] */
);


#endif