# Evapotranspiration

## 1. Conceptualization
1. An individual grid cell may contain an overstory canopy and either an
understory or bare soil. 
2. Both the understory and the overstory may
contain wet and dry fractions. 
3. The overstory, if present, may cover all or
a prescribed fraction of the cell area, while the understory or bare soil
covers the entire cell. 
4. When a ground snowpack is present, it is assumed
to completely cover the understory or soil. As a result the understory or
soil does not contribute evapotranspiration when a ground snowpack is
present.

## 2. Main procedures

Potential evaporation is first calculated for the overstory and
represents the maximum rate that water can be removed from the cell
vegetation and soil via evapotranspiration. 

Water intercepted by the
overstory is then removed from the wet fraction at the potential rate,
while transpiration from the dry fraction is modeled using a PenmanMonteith approach. 

The calculated overstory evapotranspiration (both
wet and dry fractions) is then subtracted from the potential evaporation
and understory evapotranspiration is calculated from the modified
potential evaporation rate. 

This stepwise approach allows the wet
fractions of both canopies to dry during a time step (i.e., evaporation
followed by transpiration), while insuring the total evapotranspiration
from both layers does not exceed the amount of moisture the
atmosphere can absorb (i.e., the rate of potential evaporation for the
overstory).

### 2.1 Potential evaporation for **overstory**

The rate of potential evaporation for the overstory $E_{po}$ is given by
(Wigmosta et al., 1994):

$$
E_{po} = \frac{
    \Delta \times R_{no} + \rho c_p(e_s - e) / r_{ao}
}{
    \lambda_{\nu} [\Delta + \gamma] 
}
$$

where:
- $\Delta$: the slope of the saturated vapor pressure - temperature curve,
- $R_{no}$: the net radiation flux density
- $\rho$: the density of moist air
- $c_p$: the specific heat of air at constant pressure
- $e_s$ and $e$: the saturation vapor pressure at the air temperature and the (actual) vapor pressure 
- $r_{ao}$: the aerodynamic resistance to vapor transport between the overstory and the reference height
- $\lambda_{\nu}$: the latent heat of vaporization of water
- $\gamma$: the psychrometric constant

### 2.2 Transpiration from dry vegetative surfaces

Transpiration from dry vegetative surfaces
is calculated using a Penman-Monteith approach (Wigmosta et al.,
1994)

