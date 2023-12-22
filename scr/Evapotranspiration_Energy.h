#ifndef ET_ENERGY
#define ET_ENERGY


void Radiation_net(
    double Rs,
    double L_sky,
    double *Rno,
    double *Rno_short,
    double *Rnu,
    double *Rnu_short,
    double *Rns,
    double Frac_canopy,
    double Ref_o,
    double Ref_u,
    double Ref_s,
    double Tem_o,
    double Tem_u,
    double Tem_s,
    double LAI_o,
    double LAI_u,
    int Toggle_Understory
);

double Emissivity(
    double Tem   /* temperature of the unity, [celsuis degree] */
);


#endif