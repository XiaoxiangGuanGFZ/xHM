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
- $E_{po}$: $kg/(m^2 \cdot h) = 10^3m/h$ 
- $\Delta$: the slope of the saturated vapor pressure-temperature curve, [kPa/°C]
- $R_{no}$: the net radiation flux density, [kJ/h/m2]
- $\rho$: the density of moist air, assumed as 1.2922 [kg/m3]
- $c_p$: the specific heat of air at constant pressure, 1.005 [kJ/(kg°C)]
- $e_s$ and $e$: the saturation vapor pressure at the air temperature and the (actual) vapor pressure, [kPa] 
- $r_{ao}$: the aerodynamic resistance to vapor transport between the overstory and the reference height, [h/m]
- $\lambda_{\nu}$: the latent heat of vaporization of water, set as 2500 kJ/kg (although it changes with water temperature)
- $\gamma$: the [psychrometric constant](https://en.wikipedia.org/wiki/Psychrometric_constant), [kPa/°C]

### 2.2 Transpiration from dry vegetative surfaces

Transpiration from dry vegetative surfaces
is calculated using a Penman-Monteith approach (Wigmosta et al., 1994)

$$
E_{tj} = E_{pj} \frac{
\Delta + \gamma
}{
\Delta + \gamma ( 1 + r_{cj} / r_{aj} )
}
$$

where:
- $E_{tj}$: the transpiration rate, [m/h]
- $E_{pj}$: the appropriate rate of potential evaporation, [m/h]
- $r_{cj}$: the canopy resistance to vapor transport, [h/m]
- $r_{aj}$: the aerodynamic resistance to vapor transport, [h/m]
- subscript $j$ denotes separate value for the overstory ($j=o$) and the understory($j=u$).

### 2.3 Wet-dry partition

Each canopy layer is partitioned into a wet fraction $A_{wj}$ 
and a dry fraction $1 - A_{wj}$ following Dickinson et al (1993), with:

$$
A_{wj} = ({
    \frac{
        S_{Ij}^{t} + P_j
    }{
        I_{cj}
    }
})^{2/3}
$$

where:
- $S_{Ij}^{t}$: the depth of intercepted water stored on the canopy at the start of the time step, [m]
- $P_j$: the depth of rainfall during the time step, [m/h]
- $I_{cj}$: the maximum interception storage capacity (in meters), determined from projected Leaf Area Index (LAI) [dimensionless]:

$$
    I_{cj} = r_j \cdot LAI_j \cdot F_j
$$

where:
- $r_j$: the ratio of $I_{cj}$ to $LAI_j$ (typically about $10^{-4}$ , Dickinson et al., 1991)
- $F_j$: the fraction of ground surface covered by the canopy

Rainfall is stored on the surface of each canopy until its 
maximum interception storage capacity is reached. Any excess 
precipitation passes through the canopy as throughfall with no 
attenuation.

$P$ is equal to the overstory throughfall when 
applied to the understory ($j=u$).

### 2.4 Evapotranspiration calculation for wet/dry part

The model calculates evaporation and transpiration independently
for each layer in a stepwise fashion.

First, water from the wet fraction is
evaporated at the potential rate:

$$
    E_{Ij} = E_{pj} \cdot A_{wj} \cdot \Delta t_w
$$

where:
- $E_{Ij}$: the amount of intercepted water evaporated over the time period
- $\Delta t_w$: the time required to evaporate the intercepted water at the potential rate. If the potential rate is insufficient to remove all of the intercepted water during the model time step, $\Delta t_w$ equals the length of the model time step ($\Delta t$).

The total transpiration from dry vegetation ($E_{Tj}$) is then calculated as:

$$
E_{Tj} = E_{tj} \cdot (1 - A_{wj}) \cdot \Delta t_w + 
    E_{tj} \cdot A_{wj} \cdot ( \Delta t - \Delta t_w)
$$

In practice,
- for overstory, $E_{pj} = E_{po}$
- for understory, $E_{pj} = E_{po} - (E_{Io} + E_{To})$, where $E_{Io}$ and $E_{To}$ are evaporation and transpiration from overstory respectively

### 2.5 Soil evaporation

Evaporation from the upper soil layer is simulated in the 
absence of an understory, otherwise soil evaporation is ignored.

When wet, a soil may be able to supply water to the surface at a 
rate equal to or greater than the potential evaporation demand. 
As soil moisture is depleted, the rate of delivery falls below 
the potential evaporation rate. At this and lower moisture 
states, the evaporation rate is soil-controlled and is a
nonlinear function of the soil moisture content. Under this 
approach, soil water evaporation ($E_s$) is calculated as

$$
    E_s = \min (E_{ps}, F_e)
$$

$$
    E_{ps} = E_{po} - (E_{Io} + E_{To})
$$

where the $F_e$ is the soil desorptivity, determined
by the rate at which the soil can deliver water to the surface.
Desorptivity is a function of soil type and moisture conditions 
in the upper soil zone.

## 3. Aerodynamic and Canopy Resistance

### 3.1 Aerodynamic Resistance

The vertical wind profile through the overstory canopy is modeled
assuming neutral atmospheric conditions using three layers (Storck,
2000). The wind profile above the canopy is represented using a
logarithmic profile from the reference height down to the top of a
roughness sublayer just above the overstory canopy. This sublayer
extends down to the overstory canopy displacement height. The wind
profile through the remainder of the overstory is assumed to be
exponential. Within the trunk space below the overstory canopy the
wind profile is again assumed to be logarithmic.

The total aerodynamic resistance to turbulent transport associated
with the three overstory wind profile layers is given by (Storck, 2000):

$$
    R_{ao} = \frac{
        \ln( \frac{z_r - d_o}{z_{0o}} )
    }{
        U_rk^2
    } \cdot 
    [
        \frac{h_o}{n_a(z_w - d_o)} [\exp(n_a \cdot (1 - \frac{d_o - z_{0o}}{h_o})) - 1] + 
        \frac{z_w - h_o}{z_w - d_o} +
        \ln(
            \frac{z_r - d_o}{z_w - d_o}
        )
        ]
$$

where:
- $U_r$: velocity at the reference height $z_r$
- $h_o$: height of the overstory
- $d_o$: displacement height of the overstory
- $z_{0o}$: the roughness of the overstory
- $z_w$: the height of the boundary between the upper logarithmic profile and roughness sublayer, Van de Griend and van Boxel (1989) suggested that $z_w = 1.5h_o-0.5d_o$
- $n_a$: a dimensionless extinction coefficient, with a typical value of 2-3
- $k$: Von Karmen's constant, assumed as 0.4

The aerodynamic resistance for the **soil surface**, **snow**, or **understory** 
associated with the lower logarithmic profile is given by (Storck, 2000):

$$
    r_{au} = \frac{
        \ln(\frac{z_a - d_u}{z_{0u}}) ^2 
    }{
        U(z_a) \cdot k^2
    }
$$

where $z_a = 2 + d_u + z_{0u}$ with $d_u$ and $z_{0u}$ equal to the displacement and roughness height, respectively, for the understory, soil surface, or snow surface.


### 3.2 Canopy Resistance

Canopy resistance ($r_{cj}$) is calculated separately for the overs tory and understory following the general approach of Wigmosta et al. (1994). For both stories $r_{cj}$ is represented as a summation of the stomatal resistance, $r_{cj}$ of individual leaves. The leaves are assumed to contribute in parallel so that

$$
    r_{cj}=\langle r_{sj} \rangle / [c_j \cdot LAI_j]
$$

where:
- $c_j$ is the appropriate ratio of total to projected (one-sided) LAI
- the angle brackets denote an inverse average over the
range of the canopy leaf area index ($L$) [Dickinson et al., 1991]:

$$
    \langle () \rangle = \frac{LAI}{\int^L{ d LAI/()} }
$$


The dependence of $r_{sj}$ on vegetation type and environmental
factors is represented by a **species**-dependent minimum
resistance ($r_{s \min j}$) and a product of four limiting 
factor each with a minimum value of one:

$$
    r_{sj} = r_{s \min j} \cdot f_1(T_j) \cdot f_2(vpd_j) \cdot f_3(PAR_j) \cdot f_4( {\theta}_j )
$$

where the environmental dependencies are: 
- $f_1$: air temperature 
- $f_2$: vapor pressure deficit 
- $f_3$: photosynthetically active radiation flux
- $f_4$: soil moisture

Relations for the first three of these factors are taken 
from Dickinson et al. (1993) as:

$$
    f_1 = [0.08 T_a - 0.0016 T_a^2] ^{-1}
$$

$$
    f_2 = [1 - (e_s - e)/e_c] ^{-1}
$$

$$
    f_3 = \frac{
        1 + R_p / R_{pc}
    }{
        r_{s\min} / r_{s\max} + R_p / R_{pc}
    }
$$

where 
- $T_a$: the air temperature (in degree Celsius)
- $e_c$: the vapor pressure deficit causing stomatal closure (about 4 kPa)
- $r_{s\max}$: maximum (cuticular) resistance
- $R_p$: the visible radiation, assumes as half of net shortwave radiation, [w/m2]
- $R_{pc}$: the light level where $r_s$ is twice the minimum stomatal resistance $r_{s\min}$

Following Feddes et al. (1978), $f_4$ is modeled as a piecewise linear
function of soil moisture:

$$
    f_4=0,\theta \leq \theta_{wp}
$$

$$
    f_4=\frac{
        \theta^* - \theta_{wp}
    }{
        \theta - \theta_{wp}
    }, \theta_{wp} < \theta \leq \theta^*
$$

$$
    f_4=1, \theta^* < \theta \leq \theta_s
$$

where:
- $\theta$: the average soil moisture content
- $\theta_{wp}$: is the plant wilting point
- $\theta^*$: the moisture content (field capacity) above which soil conditions do not restrict transpiration

## 4. Net radiation
### 4.1 overstory

Separate shortwave and longwave radiation budgets are developed for
the overstory and the understory or soil surface. The overstory receives
direct solar (shortwave) radiation, and exchanges longwave radiation
with both the sky and with the understory, snowpack, or soil.

The net radiation absorbed by the overstory $R_{no}$ is given by

$$
    R_{no} = R_s [(1-\alpha_o) - \tau_o(1 - \alpha_u)] \cdot F +
    (L_d + L_u - 2L_o) \cdot F
$$


where:
- $R_s$: the incident shortwave radiation
- $\alpha_o$: the overstory reflection coefficient
- $\alpha_u$: the understory reflection coefficient
- $\tau_o$: the fraction of shortwave radiation transmitted by the overstory canopy
- $F$: the fractional ground cover of the overstory
- $L_d, L_u, L_o$: downward sky, upward understory, and overstory longwave radiation fluxes

The fraction of transmitted
shortwave radiation is calculated following a Beer's Law relationship of
the form (Monteith and Unsworth, 1990):

$$
    \tau_o = \exp(-k_b \cdot LAI_o)
$$

where:
- $k_b$: a canopy attenuation coefficient
- $LAI_o$: the one-sided leaf area index of the overstory canopy

### 4.2 understory

The understory receives attenuated shortwave radiation below the
overstory and direct shortwave radiation in the open. Below the
overs tory, the understory exchanges longwave radiation with the
overstory, while in the open it exchanges radiation with the sky and
ground. The net radiation absorbed by the understory $R_{nu}$:

$$
    R_{nu} = R_s \cdot [(1-\alpha_u) - \tau_u \cdot (1 - \alpha_g)] \cdot [(1-F) + \tau_o \cdot F] + 
        (1-F) \cdot L_d + F \cdot L_o - L_u
$$

where:
- $\tau_u$: the fraction of shortwave radiation transmitted by the understory (lower canopy)
- $\alpha_g$: the soil reflectionc oefficient

### 4.3 Emissivity of unity

$$
    L_o = \sigma (T_o + 273.15)^4
$$


$$
    L_u = \sigma (T_u + 273.15)^4
$$

where:
- $\sigma$: the Stefan-Boltzmann constant
- $T_o, T_u$: the temperatures [°C] of the overstory and understory, respectively. These two temperatures are set equal to the air temperature except when snow is present.

### 4.4 Soil 
Shortwave radiation absorbed by the soil surface ($R_{sg}$) is given by:

$$
    R_{sg} = R_s \cdot \tau_u (1 - \alpha_g) \cdot 
        [(1-F) + \tau_o F]
$$

With bare soil (no understory) and no snowpack, 
the longwave radiation emitted from soil:

$$
    L_u = \sigma (T_g + 273.15)^4
$$

where:
$T_g$ equals to the soil surface temperatUre. 
The soil surface temperature is either set equal to the air temperature or when more 
accurate surface temperatures are required, calculated through an 
iterative solution to the nonlinear equation for surface temperature.


## 5. Parameters

### 5.1 Vegetation parameter

|Parameter | Grass | Aspen | Subalpine Fir | Pine |
|----------| ------| ------ | ------------ |-------|
|Summer LAI| 1.3 | 4.0| 4.0| 3.0  |
|Winter LAI| 1.3 | 0.5| 4.0| 3.0  |
| $\alpha$ | 0.18 | 0.18| 0.18 | 0.18|
| $r_{s\min}$, s/cm |  1.2 |  5.0 | 25.0 |  8.0|
| $r_{s\max}$, s/cm | 50.0 | 50.0 | 50.0 | 50.0|
| $R_{pc}$, W/m2 | 30.0 | 30.0 | 30.0 | 30.0|



## 5. References
Dickinson, R. E., A. Henderson-Sellers, C. Rosenzweig, and P. J. Sellers,
Evapotranspiration models with canopy resistance for use in climate models, a
review, Agric. For. Meteorol., 54, 373-388, 1991.

Dickinson, R. E., A. Henderson-Sellers, and P. J. Kennedy, Biosphere-atmosphere
transfer scheme (BATS) Version leas coupled to the NCAR Community Climate
Model, NCAR Technical Note, NCARITN-387+STR, Boulder, Colorado, 1993.

Feddes, R. A., P. J. Kowalik, and H. Zaradny, Simulation of field water use and crop
yield, John Wiley and Sons, New York, 188 pp., 1978.

Monteith, J. L. and M. H. Unsworth, Principles of environmental physics, Routledge,
Chapman and Hall, New York, NY, 291 pp., 1990.

Storck, P., Trees, snow and flooding: an investigation of forest canopy effects on snow
accumulation and melt at the plot and watershed scales in the Pacific Northwest,
Water Resource Series, Technical Report 161, Dept. of Civil Engineering,
University of Washington, 2000.

Wigmosta, M.S., L. W. Vail, and D. P. Lettenmaier, A distributed hydrologyvegetation
model for complex terrain, Water Resources Research, 30 (6), 1665-1679, 1994.


